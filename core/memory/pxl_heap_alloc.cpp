/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                               *
*                                                                                *
*  MIT License                                                                   *
*                                                                                *
*  Permission is hereby granted, free of charge, to any person obtaining a copy  *
*  of this software and associated documentation files (the "Software"), to      *
*  deal in the Software without restriction, including without limitation the    *
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or   *
*  sell copies of the Software, and to permit persons to whom the Software is    *
*  furnished to do so, subject to the following conditions:                      *
*                                                                                *
*  The above copyright notice and this permission notice shall be included in    *
*  all copies or substantial portions of the Software.                           *
*                                                                                *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL       *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
*  THE SOFTWARE.                                                                 *
*                                                                                *
**********************************************************************************/

#include "pxl_memory.h"

#ifndef PXL_MIN_SPLIT
#define PXL_MIN_SPLIT 32
#endif 

#ifndef PXL_ENABLE_STATS
#define PXL_ENABLE_STATS 1
#endif

struct Block {
    size_t  size;
    bool    free;
    Block*  next;
    Block*  chunk_end;
};

static std::atomic_flag global_heap_lock = ATOMIC_FLAG_INIT;

static Block* global_bins[BLOCK_COUNT] = {};
static Block* global_large_blocks = nullptr;

#if PXL_ENABLE_STATS
static size_t global_bytes_allocated    = 0;
static size_t global_peak_bytes         = 0;
static size_t global_alloc_count        = 0;
#endif


static inline void lock_heap() {
    while(global_heap_lock.test_and_set(std::memory_order_acquire)) {}
}

static inline void unlock_heap() {
    global_heap_lock.clear(std::memory_order_release);
}

static inline size_t align_up(size_t v, size_t a) {
    return (v + a - 1) & ~(a - 1);
}

static inline Block* next_physical(Block* block) {
    Block* next = (Block*)((char*)(block + 1) + block->size); 
    return (next < block->chunk_end) ? next : nullptr;
}

static inline bool is_valid_block(Block* block) {
    return block && block->size > 0;
}

static int bin_index(size_t size) {
    for(size_t i = 0; i < BLOCK_COUNT; i++)
        if(size <= BLOCK_SIZES[i]) return (int)i;
    return -1;
}

static void insert_large_block(Block* block) {
    Block** curr = &global_large_blocks;
    while(*curr && (*curr)->size < block->size)
        curr = &(*curr)->next;

    block->next = *curr;
    *curr = block;
}

static Block* find_large_block(size_t size) {
    Block** prev = &global_large_blocks;
    Block* curr = global_large_blocks;

    while(curr) {
        if(curr->size >= size) {
            *prev = curr->next;
            curr->next = nullptr;
            return curr;
        }
        prev = &curr->next;
        curr = curr->next;
    }

    return nullptr;
}

static Block* alloc_block(size_t size) {
    size_t total = align_up(sizeof(Block) + size, ALIGNMENT);
    size_t request = align_up(total, CHUNK_SIZE);

    void* memory = os_alloc(request);
    if(!memory) 
        return nullptr;

    Block* block = (Block*)memory;
    block->size = request - sizeof(Block);
    block->free = false;
    block->next = nullptr;
    block->chunk_end = (Block*)((char*)memory + request);
    
    return block;
}

static void split_block(Block* block, size_t size) {
    size_t remaining = block->size - size;

    if(remaining < sizeof(Block) + PXL_MIN_SPLIT)
        return;

    Block* split = (Block*)((char*)(block + 1) + size);
    split->size = remaining - sizeof(Block);
    split->free = true;
    split->next = nullptr;

    block->size = size;

    int bin = bin_index(split->size);
    if(bin >= 0) {
        split->next = global_bins[bin];
        global_bins[bin] = split;
    } else {
        insert_large_block(split);
    }
}

static void remove_from_free_lists(Block* block) {
    int bin = bin_index(block->size);

    if(bin >= 0) {
        Block** curr = &global_bins[bin];
        while(*curr) {
            if(*curr == block) {
                *curr = block->next;
                return;
            }
            curr = &(*curr)->next;
        }
    } else {
        Block** curr = &global_large_blocks;
        while(*curr) {
            if(*curr == block) {
                *curr = block->next;
                return;
            }
            curr = &(*curr)->next;
        }
    }
}

static Block* coalesce(Block* block) {
    Block* next = next_physical(block);

    if(is_valid_block(next) && next->free) {
        remove_from_free_lists(next);

        block->size += sizeof(Block) + next->size;
        block->next = next->next;
    }

    return block;
}

void* __pxl_malloc(size_t size) {
    if(size <= 0) return nullptr;

    size = align_up(size, ALIGNMENT);

    lock_heap();

    int bin = bin_index(size);
    if(bin >= 0 && global_bins[bin]) {
        Block* block = global_bins[bin];
        global_bins[bin] = block->next;
        block->free = false;

#if PXL_ENABLE_STATS
        global_bytes_allocated += block->size;
        global_peak_bytes = std::max(global_peak_bytes, global_bytes_allocated);
        global_alloc_count++;
#endif
        unlock_heap();
        return block + 1;
    }

    Block* block = find_large_block(size);
    if(!block) {
        block = alloc_block(size);
        if(!block) {
            unlock_heap();
            return nullptr;
        }
    }

    block->free = false;
    split_block(block, size);

#if PXL_ENABLE_STATS    
    global_bytes_allocated += block->size;
    global_peak_bytes = std::max(global_peak_bytes, global_bytes_allocated);
    global_alloc_count++;
#endif

    unlock_heap();
    return block + 1;
}

void* __pxl_realloc(void* ptr, size_t new_size) {
    if(!ptr) return __pxl_malloc(new_size);

    if(new_size == 0) {
        __pxl_free(ptr);
        return nullptr;
    }

    new_size = align_up(new_size, ALIGNMENT);
    Block* block = ((Block*)ptr) - 1;

    if(block->size >= new_size)
        return ptr;

    lock_heap();

    Block* next = next_physical(block);
    if(is_valid_block(next) && next->free &&
        block->size + sizeof(Block) + next->size >= new_size) {
        
        block->size += sizeof(Block) + next->size;
        split_block(block, new_size);
        unlock_heap();
        return ptr;
    }

    unlock_heap();

    void* new_ptr = __pxl_malloc(new_size);
    if(new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        __pxl_free(ptr);
    }

    return new_ptr;
}

void* __pxl_calloc(size_t num, size_t size) {
    if(num && size > SIZE_MAX / num)
        return nullptr;

    size_t total = num * size;

    void* ptr = __pxl_malloc(total);
    if(ptr) memset(ptr, 0, total);
    return ptr;
}

void __pxl_free(void* ptr) {
    if(!ptr) return;
    
    Block* block = ((Block*)ptr) - 1;
    assert(!block->free && "double free");

#if PXL_ENABLE_STATS
    global_bytes_allocated -= block->size;
#endif

    lock_heap();
    block->free = true;
    block = coalesce(block);

    int bin = bin_index(block->size);
    if(bin >= 0) {
        block->next = global_bins[bin];
        global_bins[bin] = block;
    } else {
        insert_large_block(block);
    }

    unlock_heap();
}

#if PXL_ENABLE_STATS

size_t pxl_allocated_bytes() {
    return global_bytes_allocated;
}

size_t pxl_peak_bytes() {
    return global_peak_bytes;
}

size_t pxl_alloc_count() {
    return global_alloc_count;
}

#endif