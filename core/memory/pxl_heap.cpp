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

#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>

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
constexpr size_t SIZE_CLASSES[NUM_CLASSES] = {32, 64, 128, 256, 512};

struct MemoryStats {
    std::atomic<size_t> user_allocated{0};
    std::atomic<size_t> user_freed{0};
    std::atomic<size_t> os_allocated{0};
    std::atomic<size_t> os_freed{0};
    std::atomic<size_t> active_allocs{0};
};

static MemoryStats g_stats;

static inline size_t align_up(size_t size, size_t alignment = ALIGNMENT) {
    return (size + (alignment - 1)) & ~(alignment - 1);
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
    void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) p = nullptr;
#endif
    if (p) g_stats.os_allocated.fetch_add(size, std::memory_order_relaxed);
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
    g_stats.os_freed.fetch_add(size, std::memory_order_relaxed);
}

struct slab;

struct header {
    size_t size;
    bool large;
#ifdef PXL_DEBUG
    uint32_t canary;
#endif
};

struct block {
    block* next;
};

struct slab {
    void* memory;
    block* free_list;
    std::atomic<block*> remote_free;
    std::atomic<size_t> remote_count{0};
    size_t total;
    size_t free;
    slab* next;
    uint64_t owner_thread;
    std::atomic<bool> eligible_for_reclaim{false};
};

thread_local slab* slabs[NUM_CLASSES] = {nullptr};
thread_local block* fast_free[NUM_CLASSES] = {nullptr};
thread_local size_t alloc_count[NUM_CLASSES] = {0};

static slab* create_slab(size_t ci) {
    size_t block_size = align_up(SIZE_CLASSES[ci] + sizeof(header));
    size_t count = SLAB_SIZE / block_size;

    void* mem = os_alloc(SLAB_SIZE);
    if (!mem) return nullptr;

    slab* s = (slab*)os_alloc(sizeof(slab));
    if (!s) { os_free(mem, SLAB_SIZE); return nullptr; }

    s->memory = mem;
    s->free_list = nullptr;
    s->remote_free.store(nullptr, std::memory_order_relaxed);
    s->remote_count.store(0, std::memory_order_relaxed);
    s->total = count;
    s->free = count;
    s->owner_thread = thread_id();
    s->eligible_for_reclaim.store(false, std::memory_order_relaxed);

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

static void sweep_slabs(size_t ci) {
    slab* prev = nullptr;
    slab* s = slabs[ci];
    while (s) {
        drain_remote(s);
        slab* next = s->next;
        if (s->eligible_for_reclaim.load(std::memory_order_acquire) && s->free == s->total) {
            if (prev) prev->next = next; else slabs[ci] = next;
            os_free(s->memory, SLAB_SIZE);
            os_free(s, sizeof(slab));
        } else {
            prev = s;
        }
        s = next;
    }
}

void* malloc(size_t size) {
    if (size == 0) return nullptr;
    size = align_up(size);

    size_t ci = NUM_CLASSES;
    for (size_t i = 0; i < NUM_CLASSES; ++i) if (size <= SIZE_CLASSES[i]) { ci = i; break; }

    if (ci < NUM_CLASSES) {
        if (fast_free[ci]) {
            block* b = fast_free[ci];
            fast_free[ci] = b->next;

            header* h = (header*)b;
            h->size = size;
            h->large = false;
#ifdef PXL_DEBUG
            h->canary = 0xDEADC0DE;
#endif
            g_stats.user_allocated.fetch_add(size, std::memory_order_relaxed);
            g_stats.active_allocs.fetch_add(1, std::memory_order_relaxed);
            return (void*)(h + 1);
        }

        slab* s = slabs[ci];
        while (s) { drain_remote(s); if (s->free_list) break; s = s->next; }
        if (!s && !(s = create_slab(ci))) return nullptr;

        block* b = s->free_list;
        s->free_list = b->next;
        s->free--;

        header* h = (header*)b;
        h->size = size;
        h->large = false;
#ifdef PXL_DEBUG
        h->canary = 0xDEADC0DE;
#endif
        g_stats.user_allocated.fetch_add(size, std::memory_order_relaxed);
        g_stats.active_allocs.fetch_add(1, std::memory_order_relaxed);

        alloc_count[ci]++;
        if (alloc_count[ci] >= 64) { sweep_slabs(ci); alloc_count[ci] = 0; }

        return (void*)(h + 1);
    }

    size_t total = align_up(size + sizeof(header), 4096);
    header* h = (header*)os_alloc(total);
    if (!h) return nullptr;

    h->size = size;
    h->large = true;
#ifdef PXL_DEBUG
    h->canary = 0xDEADC0DE;
#endif
    g_stats.user_allocated.fetch_add(size, std::memory_order_relaxed);
    g_stats.active_allocs.fetch_add(1, std::memory_order_relaxed);

    return (void*)(h + 1);
}

void free(void* ptr) {
    if (!ptr) return;

    header* h = ((header*)ptr) - 1;

#ifdef PXL_DEBUG
    if (h->canary != 0xDEADC0DE) __debugbreak();
#endif

    g_stats.user_freed.fetch_add(h->size, std::memory_order_relaxed);
    g_stats.active_allocs.fetch_sub(1, std::memory_order_relaxed);

    if (h->large) {
        os_free(h, align_up(h->size + sizeof(header), 4096));
        return;
    }

    block* b = (block*)h;

    size_t ci = NUM_CLASSES;
    for (size_t i = 0; i < NUM_CLASSES; ++i) if (h->size <= SIZE_CLASSES[i]) { ci = i; break; }
    if (ci == NUM_CLASSES) return;

    size_t count = 0;
    block* it = fast_free[ci];
    while (it) { it = it->next; count++; }
    if (count < 32) { b->next = fast_free[ci]; fast_free[ci] = b; return; }

    slab* s = slabs[ci];
    if (!s) return;
    drain_remote(s);

    b->next = s->free_list;
    s->free_list = b;
    s->free++;

    if (s->free == s->total) s->eligible_for_reclaim.store(true, std::memory_order_release);
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);

    header* h = ((header*)ptr) - 1;
    if (h->size >= size) return ptr;

    void* np = malloc(size);
    if (np) { std::memcpy(np, ptr, h->size); free(ptr); }
    return np;
}

void* calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = malloc(total);
    if (ptr) std::memset(ptr, 0, total);
    return ptr;
}

void* memalign(size_t alignment, size_t size) {
    if ((alignment & (alignment - 1)) != 0) return nullptr;

    if (alignment <= ALIGNMENT) return malloc(size);

    size_t total = size + alignment + sizeof(void*);
    void* raw = malloc(total);
    if (!raw) return nullptr;

    uintptr_t raw_addr = reinterpret_cast<uintptr_t>(raw) + sizeof(void*);
    uintptr_t aligned = (raw_addr + alignment - 1) & ~(alignment - 1);

    void** store = reinterpret_cast<void**>(aligned - sizeof(void*));
    *store = raw;
    return reinterpret_cast<void*>(aligned);
}

void aligned_free(void* ptr) {
    if (!ptr) return;
    void** store = reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ptr) - sizeof(void*));
    void* raw = *store;
    if (raw) free(raw);
    else free(ptr);
}

char* strdup(const char* s) {
    if (!s) return nullptr;
    size_t len = std::strlen(s) + 1;
    char* copy = (char*)malloc(len);
    if (copy) std::memcpy(copy, s, len);
    return copy;
}

void dump_memory_stats() {
    size_t user_live = g_stats.user_allocated.load() - g_stats.user_freed.load();
    size_t os_live = g_stats.os_allocated.load() - g_stats.os_freed.load();

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
