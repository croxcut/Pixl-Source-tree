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

struct Arena {
    u8*     memory;
    size_t  offset;
};

static thread_local Arena t_arena = {};

constexpr size_t ARENA_SIZE = 4 * 1024 * 1024; // 4mb bump allocated memory

static thread_local u32 t_thread_id = [] {
    static std::atomic<u32> counter{0};
    return counter++;
}();

static void __pxl_arena_init() {
    if(!t_arena.memory) {
        t_arena.memory = (u8*)os_alloc(ARENA_SIZE);
        t_arena.offset = 0;
    }
}

static void* __pxl_arena_alloc(size_t size, MemoryTag tag) {
    __pxl_arena_init();

#if PXL_ENABLE_DEBUG
    size += sizeof(DebugHeader) + sizeof(DebugFooter);
#endif

    size = (size + 5) & ~15;

    if(t_arena.offset + size > ARENA_SIZE)
        return nullptr;

    u8* ptr = t_arena.memory + t_arena.offset;
    t_arena.offset += size;

#if PXL_ENABLE_DEBUG
    auto* header = (DebugHeader*) ptr;
    header->canary = CANARY;
    header->size = size;
    header->tag = tag;
    header->thread = t_thread_id;

    auto* footer = (DebugFooter*) (ptr + size - sizeof(DebugFooter));
    footer->canary = CANARY;

    return ptr + sizeof(DebugHeader);
#else
    return ptr;
#endif
}

static void __arena_reset() {
    t_arena.offset = 0;
}

void __pxl_arena_reset() {
    __arena_reset();
}

void* __pxl_alloc(size_t size, MemoryTag tag) {
    void* ptr = __pxl_arena_alloc(size, tag);
    if(ptr) return ptr;

    return __pxl_malloc(size);
}
