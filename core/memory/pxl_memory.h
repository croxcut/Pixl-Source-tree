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

#ifndef PXL_MEMORY_H
#define PXL_MEMORY_H

#include "misc/utility/pxl_pre_compile.h"
#include "misc/utility/types.h"

// Windows and posix memory kernel calls :)
inline static void* os_alloc(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (ptr == MAP_FAILED) ? nullptr : ptr;
#endif
}

inline static void os_free(void* ptr, size_t size) {
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

#define PXL_ENABLE_DEBUG    0x001
#define PXL_ENABLE_NUMA     0x001
#define PXL_MAX_THREADS     0x0040

constexpr size_t PAGE_SIZE =        4096;
constexpr u64 CANARY =              0xDEADC0DECAFEBABE;

constexpr size_t KB =               1024;
constexpr size_t MB =               KB * KB;

constexpr size_t CHUNK_SIZE =       1 * MB;
constexpr size_t ALIGNMENT =        alignof(std::max_align_t);

constexpr size_t BLOCK_COUNT =      8;
constexpr size_t BLOCK_SIZES[BLOCK_COUNT] = {
    32, 64, 128, 256, 512, 1024, 2048, 4096
};

enum class MemoryTag : u8 {
    UNKNOWN,
    TEMP,
    ECS,
    RENDERER,
    PHYSICS,
    AUDIO,
    UI,
    COUNT
};

#if PXL_ENABLE_DEBUG
    struct DebugHeader{
        u64         canary;
        size_t      size;
        MemoryTag   tag;
        u32         thread;
    };

    struct DebugFooter {
        u64         canary;
    };
#endif

void*   __pxl_malloc(size_t size);
void*   __pxl_realloc(void* ptr, size_t new_size);
void*   __pxl_calloc(size_t num, size_t size);
void    __pxl_free(void* ptr);

void*   __pxl_arena_alloc(size_t size, MemoryTag tag);
void    __pxl_arena_reset();

#define pmalloc(size)               __pxl_malloc(size)
#define prealloc(ptr, new_size)     __pxl_realloc(ptr, new_size)
#define pcalloc(num, size)          __pxl_calloc(num, size)
#define pfree(ptr)                  __pxl_free(ptr)

#define palloc(size, tag)           __pxl_arena_alloc(size, tag)
#define preset()                    __pxl_arena_reset()

#endif  