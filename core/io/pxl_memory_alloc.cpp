/*==============================================================================
    PIXL ENGINE

    Copyright (C) 2026 John Paul Valenzuela
    Author : John Paul Valenzuela
    Email  : johnpaulvalenzuelaog@gmail.com

    This file is part of PIXL ENGINE.

    PIXL ENGINE is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXL ENGINE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXL ENGINE.  If not, see <https://www.gnu.org/licenses/>.
==============================================================================*/

#include "pxl_memory_alloc.h"

#include <cstdint>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

constexpr size_t ALIGNMENT   = 16;
constexpr size_t SLAB_SIZE   = 64 * 1024;
constexpr size_t NUM_CLASSES = 5;
constexpr size_t SIZE_CLASSES[NUM_CLASSES] = {
    32, 64, 128, 256, 512
};

static void* os_alloc(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? nullptr : ptr;
#endif
}

static void os_free(void* ptr, size_t size) {
#ifdef _WIN32
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

static inline size_t align_up(size_t v, size_t a) {
    return (v + (a - 1)) & ~(a - 1);
}

static inline size_t get_class_index(size_t size) {
    for (size_t i = 0; i < NUM_CLASSES; ++i)
        if (size <= SIZE_CLASSES[i])
            return i;
    return NUM_CLASSES;
}

struct px_slab;

struct alignas(ALIGNMENT) px_header {
    size_t   size;
    bool     is_large;
    uint8_t  class_index;
    px_slab* owner;
};

struct px_block {
    px_header header;
    px_block* next;
};

struct px_slab {
    void*     memory;
    px_block* free_list;
    size_t    block_size;
    size_t    total_blocks;
    size_t    free_blocks;
    px_slab*  next;
};

thread_local px_slab* slab_lists[NUM_CLASSES] = { nullptr };

static px_slab* create_slab(size_t class_index) {
    const size_t user_size  = SIZE_CLASSES[class_index];
    const size_t block_size = align_up(sizeof(px_block) + user_size, ALIGNMENT);

    void* mem = os_alloc(SLAB_SIZE);
    if (!mem) return nullptr;

    px_slab* slab = (px_slab*)os_alloc(sizeof(px_slab));
    if (!slab) {
        os_free(mem, SLAB_SIZE);
        return nullptr;
    }

    slab->memory       = mem;
    slab->block_size   = block_size;
    slab->free_list    = nullptr;
    slab->next         = nullptr;
    slab->total_blocks = SLAB_SIZE / block_size;
    slab->free_blocks  = slab->total_blocks;

    char* ptr = (char*)mem;
    for (size_t i = 0; i < slab->total_blocks; ++i) {
        px_block* blk = (px_block*)ptr;
        blk->next = slab->free_list;
        slab->free_list = blk;
        ptr += block_size;
    }

    slab->next = slab_lists[class_index];
    slab_lists[class_index] = slab;

    return slab;
}

static px_block* slab_alloc(size_t class_index) {
    px_slab* slab = slab_lists[class_index];

    while (slab) {
        if (slab->free_list) {
            px_block* blk = slab->free_list;
            slab->free_list = blk->next;
            slab->free_blocks--;
            return blk;
        }
        slab = slab->next;
    }

    slab = create_slab(class_index);
    if (!slab) return nullptr;

    px_block* blk = slab->free_list;
    slab->free_list = blk->next;
    slab->free_blocks--;
    return blk;
}

void* px_malloc(size_t size) {
    if (size == 0) return nullptr;

    size = align_up(size, ALIGNMENT);
    size_t class_index = get_class_index(size);

    // ---- SMALL ----
    if (class_index < NUM_CLASSES) {
        px_block* blk = slab_alloc(class_index);
        if (!blk) return nullptr;

        blk->header.size        = size;
        blk->header.is_large    = false;
        blk->header.class_index = (uint8_t)class_index;
        blk->header.owner       = (px_slab*)slab_lists[class_index];

        return (void*)(blk + 1);
    }

    size_t total = sizeof(px_header) + size;
    px_header* hdr = (px_header*)os_alloc(total);
    if (!hdr) return nullptr;

    hdr->size     = size;
    hdr->is_large = true;
    hdr->owner    = nullptr;

    return (void*)(hdr + 1);
}

void px_free(void* ptr) {
    if (!ptr) return;

    px_header* hdr = ((px_header*)ptr) - 1;

    if (hdr->is_large) {
        os_free(hdr, hdr->size + sizeof(px_header));
        return;
    }

    px_block* blk = (px_block*)hdr;
    px_slab* slab = hdr->owner;

    blk->next = slab->free_list;
    slab->free_list = blk;
    slab->free_blocks++;

    if (slab->free_blocks == slab->total_blocks) {
        size_t ci = hdr->class_index;
        px_slab** prev = &slab_lists[ci];

        while (*prev && *prev != slab)
            prev = &(*prev)->next;

        if (*prev)
            *prev = slab->next;

        os_free(slab->memory, SLAB_SIZE);
        os_free(slab, sizeof(px_slab));
    }
}

void* px_realloc(void* ptr, size_t new_size) {
    if (!ptr) return px_malloc(new_size);
    if (new_size == 0) {
        px_free(ptr);
        return nullptr;
    }

    px_header* hdr = ((px_header*)ptr) - 1;

    if (hdr->size >= new_size)
        return ptr;

    void* new_ptr = px_malloc(new_size);
    if (!new_ptr) return nullptr;

    std::memcpy(new_ptr, ptr, hdr->size);
    px_free(ptr);

    return new_ptr;
}
