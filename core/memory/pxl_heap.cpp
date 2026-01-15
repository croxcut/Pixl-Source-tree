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

#include "pxl_memory.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <cstdint>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <pthread.h>
#endif

namespace px {

    constexpr size_t ALIGNMENT   = 16;
    constexpr size_t SLAB_SIZE   = 64 * 1024;
    constexpr size_t NUM_CLASSES = 5;
    constexpr size_t SIZE_CLASSES[NUM_CLASSES] = { 32, 64, 128, 256, 512 };

    struct MemoryStats {
        std::atomic<size_t> user_allocated{0};
        std::atomic<size_t> user_freed{0};

        std::atomic<size_t> os_allocated{0};
        std::atomic<size_t> os_freed{0};

        std::atomic<size_t> active_allocs{0};
    };

    static MemoryStats g_stats;

    static void* os_alloc(size_t size) {
    #ifdef _WIN32
        void* p = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    #else
        void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED) p = nullptr;
    #endif
        if (p) g_stats.os_allocated += size;
        return p;
    }

    static void os_free(void* ptr, size_t size) {
        if (!ptr) return;
    #ifdef _WIN32
        (void)size;
        VirtualFree(ptr, 0, MEM_RELEASE);
    #else
        munmap(ptr, size);
    #endif
        g_stats.os_freed += size;
    }

    static inline size_t align_up(size_t v) {
        return (v + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    }

    static inline uint32_t thread_id() {
    #ifdef _WIN32
        return GetCurrentThreadId();
    #else
        return (uint32_t)pthread_self();
    #endif
    }

    struct slab;

    struct alignas(ALIGNMENT) header {
        size_t size;
        bool   large;
        uint8_t class_index;
        slab*  owner;
    #ifdef PXL_DEBUG
        uint32_t canary;
    #endif
    };

    struct block {
        header hdr;
        block* next;
    };

    struct slab {
        void* memory;
        block* free_list;
        std::atomic<block*> remote_free;
        size_t total;
        size_t free;
        slab* next;
        uint32_t owner_thread;
    };

    thread_local slab* slabs[NUM_CLASSES] = { nullptr };

    static slab* create_slab(size_t ci) {
        size_t block_size = align_up(sizeof(block) + SIZE_CLASSES[ci]);
        size_t count = SLAB_SIZE / block_size;

        void* mem = os_alloc(SLAB_SIZE);
        slab* s   = (slab*)os_alloc(sizeof(slab));

        s->memory = mem;
        s->free_list = nullptr;
        s->remote_free.store(nullptr);
        s->total = count;
        s->free = count;
        s->owner_thread = thread_id();

        char* p = (char*)mem;
        for (size_t i = 0; i < count; ++i) {
            block* b = (block*)p;
            b->next = s->free_list;
            s->free_list = b;
            p += block_size;
        }

        s->next = slabs[ci];
        slabs[ci] = s;
        return s;
    }

    static inline void drain_remote(slab* s) {
        block* head = s->remote_free.exchange(nullptr);
        while (head) {
            block* n = head->next;
            head->next = s->free_list;
            s->free_list = head;
            s->free++;
            head = n;
        }
    }

    void free(void* ptr) {
        if (!ptr) return;

        block* b = (block*)ptr - 1;
        header* h = &b->hdr;

    #ifdef PXL_DEBUG
        if (h->canary != 0xDEADC0DE)
            __debugbreak();
    #endif

        g_stats.user_freed += h->size;
        g_stats.active_allocs--;

        if (h->large) {
            os_free(h, h->size + sizeof(header));
            return;
        }

        slab* s = h->owner;
        if (s->owner_thread == thread_id()) {
            b->next = s->free_list;
            s->free_list = b;
            s->free++;
        } else {
            block* old;
            do {
                old = s->remote_free.load();
                b->next = old;
            } while (!s->remote_free.compare_exchange_weak(old, b));
        }
    }

    void* malloc(size_t size) {
        if (!size) return nullptr;
        size = align_up(size);

        size_t ci = NUM_CLASSES;
        for (size_t i = 0; i < NUM_CLASSES; ++i)
            if (size <= SIZE_CLASSES[i]) { ci = i; break; }

        if (ci < NUM_CLASSES) {
            slab* s = slabs[ci];
            while (s) {
                drain_remote(s);
                if (s->free_list) break;
                s = s->next;
            }
            if (!s) s = create_slab(ci);

            block* b = s->free_list;
            s->free_list = b->next;
            s->free--;

            b->hdr = { size, false, (uint8_t)ci, s
    #ifdef PXL_DEBUG
            , 0xDEADC0DE
    #endif
            };

            g_stats.user_allocated += size;
            g_stats.active_allocs++;

            return (void*)(b + 1);
        }

        size_t total = sizeof(header) + size;
        header* h = (header*)os_alloc(total);
        h->size = size;
        h->large = true;
        h->owner = nullptr;
    #ifdef PXL_DEBUG
        h->canary = 0xDEADC0DE;
    #endif

        g_stats.user_allocated += size;
        g_stats.active_allocs++;

        return (void*)(h + 1);
    }

    void* realloc(void* ptr, size_t size) {
        if (!ptr) return malloc(size);
        block* b = (block*)ptr - 1;
        header* h = &b->hdr;
        if (h->size >= size) return ptr;

        void* np = malloc(size);
        std::memcpy(np, ptr, h->size);
        free(ptr);
        return np;
    }

    void dump_memory_stats() {
        size_t user_live =
            g_stats.user_allocated.load() -
            g_stats.user_freed.load();

        size_t os_live =
            g_stats.os_allocated.load() -
            g_stats.os_freed.load();

        printf("========== PIXL MEMORY STATS ==========\n");
        printf("User allocated : %zu bytes\n", g_stats.user_allocated.load());
        printf("User freed     : %zu bytes\n", g_stats.user_freed.load());
        printf("User live      : %zu bytes\n\n", user_live);

        printf("OS allocated   : %zu bytes\n", g_stats.os_allocated.load());
        printf("OS freed       : %zu bytes\n", g_stats.os_freed.load());
        printf("OS live        : %zu bytes\n\n", os_live);

        printf("Active allocs  : %zu\n", g_stats.active_allocs.load());
        printf("======================================\n");
    }

} 