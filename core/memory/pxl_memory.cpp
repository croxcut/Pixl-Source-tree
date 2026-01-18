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

// Window and posix memory kernel calls :)
#ifdef _WIN32
    static void* os_alloc(size_t size) {
        return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    static void os_free(void* ptr, size_t size) {
        VirtualFree(ptr, 0, MEM_RELEASE);
    }
#else
    static void* os_alloc(size_t size) {
        void* ptr = mmap(nullptr, size, PROT_HEAD | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (ptr == MAP_FAILED) ? nullptr : ptr;
    }
    static void os_free(void* ptr, size_t size) {
        munmap(ptr, size);
    }
#endif

// Spin lock thread :)
static std::atomic_flag global_heap_lock = ATOMIC_FLAG_INIT;

static inline void lock_heap() {
    while(global_heap_lock.test_and_set(std::memory_order_acquire)) {}
}

static inline void unlock_heap() {
    global_heap_lock.clear(std::memory_order_release);
}

struct Block {
    size_t size;
    bool free;
    Block* next;
};

struct Arena {
    char* memory;
    size_t size;
    size_t offset;
};

constexpr size_t KB = 1024;
constexpr size_t MB = 1024 * KB;

constexpr size_t CHUNK_SIZE = 1 * MB;
constexpr size_t ALIGNMENT = alignof(std::max_align_t);

constexpr size_t BLOCK_COUNT = 8;
constexpr size_t BLOCK_SIZES[BLOCK_COUNT] = {
    32, 64, 128, 256, 512, 1024, 2048, 4096
};

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

static Block* alloc_block(size_t size) {
    size_t total = align_up(sizeof(Block) + size, ALIGNMENT);
    size_t request = align_up(total, CHUNK_SIZE);

    void* memory = os_alloc(request);
    if(!memory) return nullptr;

    Block* self = (Block*)memory;
    self->size = request - sizeof(Block);
    self->free = false;
    self->next = nullptr;

    return self;
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

void* _pxl_malloc(size_t size) {
    if(size == 0) return nullptr;
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

    Block* block = alloc_block(size);
    if(!block) {
        unlock_heap();
        return nullptr;
    }

    split_block(block, size);
    unlock_heap();

    return block + 1;
}

void _pxl_free(void* ptr) {
    if(!ptr) return;

    Block* block = ((Block*)ptr) - 1;
    assert(!block->free && "double free");

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

void* _pxl_realloc(void* ptr, size_t new_size) {
    if(!ptr) return _pxl_malloc(new_size);
    
    if(new_size == 0) {
        _pxl_free(ptr);
        return nullptr;
    }

    Block* block = ((Block*) ptr) - 1;
    if(block->size >= new_size) 
        return ptr;

    void* new_ptr = _pxl_malloc(new_size);
    if(new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        _pxl_free(ptr);
    }

    return new_ptr;
}

void* _pxl_calloc(size_t num, size_t size) {
    if(num && size > SIZE_MAX / num) return nullptr;
    size_t total = num * size;

    void* ptr = _pxl_malloc(total);
    if(ptr) memset(ptr, 0, total);
    return ptr;
}

Arena* _pxl_arena_create(size_t size) {
    void* memory = os_alloc(size);
    if(!memory) return nullptr;

    Arena* self = (Arena*)_pxl_malloc(sizeof(Arena));
    self->memory = (char*)memory;
    self->size = size;
    self->offset = 0;

    return self;
}

void* _pxl_arena_alloc(Arena* self, size_t size, size_t alignment = ALIGNMENT) {
    uintptr_t aligned = align_up(self->offset, alignment);

    if(aligned + size > self->size)
        return _pxl_malloc(size);

    void* ptr = self->memory + aligned;
    self->offset = aligned + size;

    return ptr; 
}

void _pxl_arena_reset(Arena* self) {
    self->offset = 0;   
}

void _pxl_arena_destroy(Arena* self) {
    os_free(self->memory, self->size);
    _pxl_free(self);
}