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

#define pmalloc(size)               _pxl_malloc(size)
#define pfree(ptr)                  _pxl_free(ptr)
#define prealloc(ptr, new_size)     _pxl_realloc(ptr, new_size)
#define pcalloc(num, size)          _pxl_calloc(num, size)

#endif  