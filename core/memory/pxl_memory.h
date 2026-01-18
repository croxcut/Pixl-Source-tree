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

struct  Arena;   // forward declaration for Arena block :)*

void*   _pxl_malloc(size_t size);
void    _pxl_free(void* ptr);
void*   _pxl_realloc(void* ptr, size_t new_size);
void*   _pxl_calloc(size_t num, size_t size);

Arena*  _pxl_arena_create(size_t size);
void*   _pxl_arena_alloc(Arena* self, size_t size, size_t alignment);
void    _pxl_arena_reset(Arena* self);
void    _pxl_arena_destroy(Arena* self);

#define pmalloc(size)                   _pxl_malloc(size)
#define pfree(ptr)                      _pxl_free(ptr)
#define prealloc(ptr, new_size)         _pxl_realloc(ptr, new_size)
#define pcalloc(num, size)              _pxl_calloc(num, size)

#define parninit(size)                  _pxl_arena_create(size)
#define parnreset(self)                 _pxl_arena_reset(self)
#define parnalloc(self, size, align)    _pxl_arena_alloc(self, size, align)
#define parndestroy(self)               _pxl_arena_destroy(self)     

#endif  