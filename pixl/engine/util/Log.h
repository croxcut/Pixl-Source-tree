#ifndef __LOG__
#define __LOG__

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <cstdlib> 

enum Level { INFO, WARN, ERROR, DEBUG};

#define LOG(lvl, fmt, ...) \
    do { \
        const char* prefix = \
            ((lvl) == INFO)  ? "INFO"  : \
            ((lvl) == WARN)  ? "WARN"  : \
            ((lvl) == DEBUG) ? "DEBUG" : \
                                "ERROR"; \
        char buffer[1024]; \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
        std::cout << "[Pixl] " << prefix << " : " << buffer << "\n"; \
    } while(0)

#define ASSERT(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            char buffer[1024]; \
            snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
            std::cerr << "[Pixl] ASSERT FAILED (" << #expr << ") : " \
                      << buffer << "\n"; \
            std::abort(); \
        } \
    } while(0)

#endif