#ifndef HANDLE_H
#define HANDLE_H

#include <atomic>
#include <random>

#include "types.h"

inline u64 generate_id() noexcept {
    static std::atomic<u64> counter{1};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

#endif