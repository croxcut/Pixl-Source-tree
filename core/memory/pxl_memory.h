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

#include "misc/utility/pre_compile.h"
#include "misc/utility/types.h"

#define PXL_ENABLE_DEBUG    0x001
#define PXL_ENABLE_NUMA     0x001
#define PXL_MAX_THREADS     0x0040

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

void*   _pxl_malloc(size_t size);
void    _pxl_free(void* ptr);
void*   _pxl_realloc(void* ptr, size_t new_size);
void*   _pxl_calloc(size_t num, size_t size);

void    __pxl_arena_reset();

void*   __pxl_alloc(size_t size, MemoryTag tag);
void    __pxl_free(void* ptr);

#define pmalloc(size)                   _pxl_malloc(size)
#define prealloc(ptr, new_size)         _pxl_realloc(ptr, new_size)
#define pcalloc(num, size)              _pxl_calloc(num, size)


#define palloc(size, tag)               __pxl_alloc(size, tag)
#define preset()                        __pxl_arena_reset()
#define pfree(ptr)                      __pxl_free(ptr)

#endif  