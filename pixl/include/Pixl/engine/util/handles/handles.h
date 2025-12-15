#ifndef __HANDLES__
#define __HANDLES__

#include <atomic>
#include <random>

#include "../Types.h"

inline u64 generateId() {
    static std::atomic<u64> counter{1};

    static u64 salt = []() {
        std::mt19937_64 rng{ std::random_device{}()};
        std::uniform_int_distribution<u64> dist;
        return dist(rng);
    }();

    return counter++ ^ salt;
}

#endif