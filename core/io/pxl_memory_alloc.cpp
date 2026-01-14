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
#include <mutex>
#include <thread>
#include <atomic>
#include <cstring>

constexpr size_t ALIGNMENT   = 16;
constexpr size_t NUM_CLASSES = 5;
constexpr size_t SLAB_SIZE   = 64 * 1024; 
constexpr size_t SIZE_CLASSES[NUM_CLASSES] = {32, 64, 128, 256, 512};

struct block {
    size_t size;
    bool is_free;
    block* next_free; 
};

struct slab {
    char* memory;
    size_t size;
    size_t block_size;
    block* free_list;  
    slab* next;
    std::mutex lock;   
    size_t free_count; 
};

struct block_header {
    size_t size;
    bool is_large;
};

thread_local slab* slab_lists[NUM_CLASSES] = {nullptr};

inline size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

inline void* align_pointer(void* ptr, size_t alignment) {
    uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
    size_t offset = (alignment - (p % alignment)) % alignment;
    return reinterpret_cast<void*>(p + offset);
}

inline size_t get_class_index(size_t size) {
    for (size_t i = 0; i < NUM_CLASSES; ++i)
        if (size <= SIZE_CLASSES[i]) return i;
    return NUM_CLASSES; 
}

inline void* os_alloc(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? nullptr : ptr;
#endif
}

inline void os_free(void* ptr, size_t size) {
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

static slab* create_slab(size_t class_index) {
    size_t block_size = SIZE_CLASSES[class_index];
    char* slab_mem = (char*)os_alloc(SLAB_SIZE);
    if (!slab_mem) return nullptr;

    slab* new_slab = new slab{slab_mem, SLAB_SIZE, block_size, nullptr, nullptr, {}, 0};

    size_t num_blocks = SLAB_SIZE / (block_size + sizeof(block));
    char* ptr = slab_mem;
    for (size_t i = 0; i < num_blocks; ++i) {
        block* blk = (block*)ptr;
        blk->size = block_size;
        blk->is_free = true;
        blk->next_free = new_slab->free_list;
        new_slab->free_list = blk;
        ++new_slab->free_count;
        ptr += sizeof(block) + block_size;
    }

    new_slab->next = slab_lists[class_index];
    slab_lists[class_index] = new_slab;

    return new_slab;
}

static block* allocate_from_slab(size_t class_index) {
    slab* slab_ptr = slab_lists[class_index];
    while (slab_ptr) {
        std::lock_guard<std::mutex> lock(slab_ptr->lock);
        if (slab_ptr->free_list) {
            block* blk = slab_ptr->free_list;
            slab_ptr->free_list = blk->next_free;
            blk->is_free = false;
            --slab_ptr->free_count;
            return blk;
        }
        slab_ptr = slab_ptr->next;
    }

    slab_ptr = create_slab(class_index);
    if (!slab_ptr) return nullptr;

    std::lock_guard<std::mutex> lock(slab_ptr->lock);
    block* blk = slab_ptr->free_list;
    slab_ptr->free_list = blk->next_free;
    blk->is_free = false;
    --slab_ptr->free_count;
    return blk;
}

void* px_malloc(size_t size) {
    if (size == 0) return nullptr;
    size = align_size(size);

    size_t class_index = get_class_index(size);

    if (class_index < NUM_CLASSES) {
        block* blk = allocate_from_slab(class_index);
        if (!blk) return nullptr;
        return align_pointer((char*)blk + sizeof(block), ALIGNMENT);
    }

    size_t total = sizeof(block_header) + size;
    block_header* hdr = (block_header*)os_alloc(total);
    if (!hdr) return nullptr;
    hdr->size = size;
    hdr->is_large = true;
    return (char*)hdr + sizeof(block_header);
}

void px_free(void* ptr) {
    if (!ptr) return;

    char* cptr = (char*)ptr;
    block_header* hdr = (block_header*)(cptr - sizeof(block_header));

    if (hdr->is_large) {
        os_free(hdr, hdr->size + sizeof(block_header));
        return;
    }

    block* blk = (block*)(cptr - sizeof(block));

    blk->is_free = true;

    for (size_t i = 0; i < NUM_CLASSES; ++i) {
        slab* s = slab_lists[i];
        while (s) {
            if ((char*)blk >= s->memory && (char*)blk < s->memory + s->size) {
                std::lock_guard<std::mutex> lock(s->lock);
                blk->next_free = s->free_list;
                s->free_list = blk;
                ++s->free_count;

                size_t num_blocks = s->size / (s->block_size + sizeof(block));
                if (s->free_count == num_blocks) {
                    slab** prev = &slab_lists[i];
                    while (*prev && *prev != s) prev = &(*prev)->next;
                    if (*prev) *prev = s->next;
                    os_free(s->memory, s->size);
                    delete s;
                }
                return;
            }
            s = s->next;
        }
    }
}

void* px_realloc(void* ptr, size_t new_size) {
    if (!ptr) return px_malloc(new_size);
    if (new_size == 0) {
        px_free(ptr);
        return nullptr;
    }

    char* cptr = (char*)ptr;
    block_header* hdr = (block_header*)(cptr - sizeof(block_header));

    if (hdr->is_large) {
        if (hdr->size >= new_size) return ptr;
        void* new_ptr = px_malloc(new_size);
        if (!new_ptr) return nullptr;
        std::memcpy(new_ptr, ptr, hdr->size);
        px_free(ptr);
        return new_ptr;
    }

    block* blk = (block*)(cptr - sizeof(block));
    if (blk->size >= new_size) return ptr; 

    void* new_ptr = px_malloc(new_size);
    if (!new_ptr) return nullptr;
    std::memcpy(new_ptr, ptr, blk->size);
    px_free(ptr);
    return new_ptr;
}
