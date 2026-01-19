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

static std::atomic_flag global_heap_lock = ATOMIC_FLAG_INIT;

static inline void lock_heap() {
    while(global_heap_lock.test_and_set(std::memory_order_acquire)) {}
}

static inline void unlock_heap() {
    global_heap_lock.clear(std::memory_order_release);
}

static Block* global_bins[BLOCK_COUNT] = {};
static Block* global_large_blocks = nullptr;

static inline size_t align_up(size_t v, size_t a) {
    return (v + a - 1) & ~(a - 1);
}

static int bin_index(size_t size) {
    for(size_t i = 0; i < BLOCK_COUNT; i++)
        if(size <= BLOCK_SIZES[i]) return i;
    return -1;
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

    return block;
}

static void split_block(Block* block, size_t size) {
    size_t remaining = block->size - size;
    if(remaining <= sizeof(Block) + ALIGNMENT) return;

    Block* new_block = (Block*)((char*)(block + 1) + size);
    new_block->size = remaining - sizeof(Block);
    new_block->free = true;
    new_block->next = nullptr;

    block->size = size;

    int bin = bin_index(new_block->size);
    if(bin >= 0) {
        new_block->next = global_bins[bin];
        global_bins[bin] = new_block;
    }
}

void* __pxl_malloc(size_t size) {
    if(size <= 0) return nullptr;

    size = align_up(size, ALIGNMENT);

    lock_heap();

    int bin = bin_index(size);

    if(bin >= 0) {
        Block* block = global_bins[bin];
        if(block) {
            global_bins[bin] = block->next;
            block->free = false;
            unlock_heap();
            return block + 1;
        }
    }

    Block* large = find_large_block(size);
    if(large) {
        large->free = false;
        unlock_heap();
        return large + 1;
    }

    Block* block = alloc_block(size);
    if(!block) {
        unlock_heap();
        return nullptr;
    }

    split_block(block, size);
    unlock_heap();

    return block + 1;
}

void* __pxl_realloc(void* ptr, size_t new_size) {
    if(!ptr) return __pxl_malloc(new_size);

    if(new_size == 0) {
        __pxl_free(ptr);
        return nullptr;
    }

    Block* block = ((Block*) ptr) - 1;
    if(block->size >= new_size)
        return ptr;

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

#if PXL_ENABLE_DEBUG
    DebugHeader* header = (DebugHeader*)((u8*)ptr - sizeof(DebugHeader));
    DebugHeader* footer = (DebugHeader*)((u8*)header + header->size - sizeof(DebugHeader));
    assert(header->canary == CANARY && "memory underrun");
    assert(footer->canary == CANARY && "memory overrun");
#endif

    lock_heap();
    block->free = true;

    int bin = bin_index(block->size);
    if(bin >= 0) {
        block->next = global_bins[bin];
        global_bins[bin] = block;
        unlock_heap();
        return;
    }

    block->next = global_large_blocks;
    global_large_blocks = block;

    unlock_heap();
}