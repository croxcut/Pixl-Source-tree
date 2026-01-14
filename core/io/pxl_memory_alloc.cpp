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
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>

constexpr size_t alignment = 16;
constexpr size_t num_classes = 5;
constexpr size_t slab_size = 64 * 1024; 
size_t size_classes[num_classes] = {32, 64, 128, 256, 512};

thread_local slab* slab_lists[num_classes] = {nullptr};

std::mutex global_mutex;

void* os_alloc(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? nullptr : ptr;
#endif
}

void os_free(void* ptr, size_t size) {
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

size_t align_size(size_t size) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

size_t get_class_index(size_t size) {
    for (size_t i = 0; i < num_classes; ++i)
        if (size <= size_classes[i]) return i;
    return num_classes; 
}

void create_slab(size_t class_index) {
    size_t block_size = size_classes[class_index];
    char* slab_memory = (char*)os_alloc(slab_size);
    if (!slab_memory) throw std::bad_alloc();

    slab* new_slab = new slab{slab_memory, slab_size, nullptr, nullptr};

    {
        std::lock_guard<std::mutex> lock(global_mutex);
        new_slab->next = slab_lists[class_index];
        slab_lists[class_index] = new_slab;
    }

    size_t num_blocks = slab_size / (block_size + sizeof(block));
    char* ptr = slab_memory;
    for (size_t i = 0; i < num_blocks; ++i) {
        block* blk = (block*)ptr;
        blk->size = block_size;
        blk->is_free = true;
        blk->next = new_slab->blocks;
        new_slab->blocks = blk;

        ptr += sizeof(block) + block_size;
    }
}

block* find_free_block(size_t class_index) {
    slab* cur_slab = slab_lists[class_index];
    while (cur_slab) {
        block* cur_block = cur_slab->blocks;
        while (cur_block) {
            if (cur_block->is_free) return cur_block;
            cur_block = cur_block->next;
        }
        cur_slab = cur_slab->next;
    }
    return nullptr;
}

void* px_malloc(size_t size) {
    size = align_size(size);
    size_t class_index = get_class_index(size);

    if (class_index < num_classes) {
        block* blk = find_free_block(class_index);
        if (!blk) create_slab(class_index);
        blk = find_free_block(class_index);
        if (!blk) return nullptr;

        blk->is_free = false;
        return (char*)blk + sizeof(block);
    } else {
        size_t total = size + sizeof(block_header);
        block_header* hdr = (block_header*)os_alloc(total);
        if (!hdr) return nullptr;
        hdr->size = size;
        hdr->is_large = true;
        return (char*)hdr + sizeof(block_header);
    }
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
}

void* px_realloc(void* ptr, size_t new_size) {
    if (!ptr) return px_malloc(new_size);

    char* cptr = (char*)ptr;

    block_header* hdr = (block_header*)(cptr - sizeof(block_header));
    if (hdr->is_large) {
        if (hdr->size >= new_size) return ptr;

        void* new_ptr = px_malloc(new_size);
        std::memcpy(new_ptr, ptr, hdr->size);
        px_free(ptr);
        return new_ptr;
    }

    block* blk = (block*)(cptr - sizeof(block));
    if (blk->size >= new_size) return ptr;

    void* new_ptr = px_malloc(new_size);
    std::memcpy(new_ptr, ptr, blk->size);
    px_free(ptr);
    return new_ptr;
}