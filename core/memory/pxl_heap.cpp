/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                                 *
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
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <pthread.h>
#endif

namespace px {

constexpr size_t ALIGNMENT        = 16;
constexpr size_t SLAB_SIZE        = 64 * 1024;
constexpr size_t FAST_FREE_LIMIT  = 32;
constexpr size_t NUM_CLASSES      = 5;
constexpr size_t SIZE_CLASSES[NUM_CLASSES] = {
    32, 64, 128, 256, 512
};

#ifdef PXL_DEBUG
constexpr uint32_t CANARY_VALUE = 0xDEADC0DE;
#endif

struct MemoryStats {
    std::atomic<size_t> user_allocated{0};
    std::atomic<size_t> user_freed{0};
    std::atomic<size_t> os_allocated{0};
    std::atomic<size_t> os_freed{0};
    std::atomic<size_t> active_allocs{0};
};

static MemoryStats g_stats;

struct slab;

struct header {
    size_t size;
    slab* owner;
    bool large;
#ifdef PXL_DEBUG
    uint32_t canary;
#endif
};

struct block {
    block* next;
};

struct slab {
    block* free_list;
    std::atomic<block*> remote_free;
    std::atomic<size_t> remote_count;
    size_t total;
    size_t free;
    size_t block_size;
    slab* next;
    uint64_t owner_thread;
};

thread_local slab* slabs[NUM_CLASSES] = { nullptr };
thread_local block* fast_free[NUM_CLASSES] = { nullptr };
thread_local size_t fast_free_count[NUM_CLASSES] = { 0 };

static inline size_t align_up(size_t v, size_t a = ALIGNMENT) {
    return (v + (a - 1)) & ~(a - 1);
}

static inline uint64_t thread_id() {
#ifdef _WIN32
    return static_cast<uint64_t>(GetCurrentThreadId());
#else
    return reinterpret_cast<uint64_t>(pthread_self());
#endif
}

static void* os_alloc(size_t size) {
#ifdef _WIN32
    void* p = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) p = nullptr;
#endif
    if (p) g_stats.os_allocated.fetch_add(size, std::memory_order_relaxed);
    return p;
}

static void os_free(void* p, size_t size) {
    if (!p) return;
#ifdef _WIN32
    (void)size;
    VirtualFree(p, 0, MEM_RELEASE);
#else
    munmap(p, size);
#endif
    g_stats.os_freed.fetch_add(size, std::memory_order_relaxed);
}

static slab* create_slab(size_t ci) {
    size_t block_size = align_up(sizeof(header) + SIZE_CLASSES[ci]);
    size_t usable     = SLAB_SIZE - align_up(sizeof(slab));
    size_t count      = usable / block_size;

    void* mem = os_alloc(SLAB_SIZE);
    if (!mem) return nullptr;

    slab* s = new (mem) slab{};
    s->block_size   = block_size;
    s->total        = count;
    s->free         = count;
    s->owner_thread = thread_id();
    s->remote_free.store(nullptr, std::memory_order_relaxed);
    s->remote_count.store(0, std::memory_order_relaxed);

    char* p = (char*)mem + align_up(sizeof(slab));
    s->free_list = nullptr;

    for (size_t i = 0; i < count; i++) {
        block* b = (block*)p;
        b->next = s->free_list;
        s->free_list = b;
        p += block_size;
    }

    s->next = slabs[ci];
    slabs[ci] = s;
    return s;
}

static void drain_remote(slab* s) {
    if (s->remote_count.load(std::memory_order_acquire) == 0) return;

    block* head = s->remote_free.exchange(nullptr, std::memory_order_acq_rel);
    s->remote_count.store(0, std::memory_order_release);

    while (head) {
        block* n = head->next;
        head->next = s->free_list;
        s->free_list = head;
        s->free++;
        head = n;
    }
}

void* malloc(size_t size) {
    if (size == 0) return nullptr;
    size = align_up(size);

    size_t ci = NUM_CLASSES;
    for (size_t i = 0; i < NUM_CLASSES; i++) {
        if (size <= SIZE_CLASSES[i]) {
            ci = i;
            break;
        }
    }

    if (ci < NUM_CLASSES) {
        if (fast_free[ci]) {
            block* b = fast_free[ci];
            fast_free[ci] = b->next;
            fast_free_count[ci]--;

            header* h = (header*)b;
            h->size  = size;
            h->large = false;

#ifdef PXL_DEBUG
            h->canary = CANARY_VALUE;
#endif
            g_stats.user_allocated.fetch_add(size);
            g_stats.active_allocs.fetch_add(1);
            return (void*)(h + 1);
        }

        slab* s = slabs[ci];
        while (s) {
            drain_remote(s);
            if (s->free_list) break;
            s = s->next;
        }

        if (!s) s = create_slab(ci);
        if (!s) return nullptr;

        block* b = s->free_list;
        s->free_list = b->next;
        s->free--;

        header* h = (header*)b;
        h->size  = size;
        h->owner = s;
        h->large = false;
#ifdef PXL_DEBUG
        h->canary = CANARY_VALUE;
#endif
        g_stats.user_allocated.fetch_add(size);
        g_stats.active_allocs.fetch_add(1);
        return (void*)(h + 1);
    }

    size_t total = align_up(size + sizeof(header), 4096);
    header* h = (header*)os_alloc(total);
    if (!h) return nullptr;

    h->size  = size;
    h->owner = nullptr;
    h->large = true;
#ifdef PXL_DEBUG
    h->canary = CANARY_VALUE;
#endif
    g_stats.user_allocated.fetch_add(size);
    g_stats.active_allocs.fetch_add(1);
    return (void*)(h + 1);
}

void free(void* ptr) {
    if (!ptr) return;
    header* h = ((header*)ptr) - 1;

#ifdef PXL_DEBUG
    if (h->canary != CANARY_VALUE) {
        std::abort();
    }
    std::memset(ptr, 0xDD, h->size);
#endif

    g_stats.user_freed.fetch_add(h->size);
    g_stats.active_allocs.fetch_sub(1);

    if (h->large) {
        os_free(h, align_up(h->size + sizeof(header), 4096));
        return;
    }

    slab* s = h->owner;
    block* b = (block*)h;

    if (s->owner_thread != thread_id()) {
        block* old = s->remote_free.load(std::memory_order_relaxed);
        do {
            b->next = old;
        } while (!s->remote_free.compare_exchange_weak(
            old, b, std::memory_order_release, std::memory_order_relaxed));
        s->remote_count.fetch_add(1);
        return;
    }

    if (fast_free_count[s->block_size <= 32 ? 0 :
                         s->block_size <= 64 ? 1 :
                         s->block_size <= 128 ? 2 :
                         s->block_size <= 256 ? 3 : 4] < FAST_FREE_LIMIT) {
        size_t ci =
            s->block_size <= 32  ? 0 :
            s->block_size <= 64  ? 1 :
            s->block_size <= 128 ? 2 :
            s->block_size <= 256 ? 3 : 4;

        b->next = fast_free[ci];
        fast_free[ci] = b;
        fast_free_count[ci]++;
        return;
    }

    drain_remote(s);
    b->next = s->free_list;
    s->free_list = b;
    s->free++;
}

void dump_memory_stats() {
    size_t cached = 0;
    for (size_t ci = 0; ci < NUM_CLASSES; ci++) {
        slab* s = slabs[ci];
        while (s) {
            cached += s->free * s->block_size;
            s = s->next;
        }
    }

    size_t user_alloc = g_stats.user_allocated.load();
    size_t user_freed = g_stats.user_freed.load();
    size_t os_alloc   = g_stats.os_allocated.load();
    size_t os_freed   = g_stats.os_freed.load();
    size_t active     = g_stats.active_allocs.load();

    size_t user_live = (user_alloc > user_freed) ? (user_alloc - user_freed) : 0;
    size_t os_live   = (os_alloc > os_freed) ? (os_alloc - os_freed) : 0;

    printf("========== PIXL MEMORY STATS ==========\n");
    printf("User allocated : %zu bytes\n", user_alloc);
    printf("User freed     : %zu bytes\n", user_freed);
    printf("Cached slabs   : %zu bytes\n", cached);
    printf("User live      : %zu bytes\n\n", user_live);
    printf("OS allocated   : %zu bytes\n", os_alloc);
    printf("OS freed       : %zu bytes\n", os_freed);
    printf("OS live        : %zu bytes\n\n", os_live);
    printf("Active allocs  : %zu\n", active);
    printf("======================================\n");
}

} 
