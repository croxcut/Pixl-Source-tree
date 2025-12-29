#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"

static inline const char* pixlShortFile(const char* path) {
    const char* src_pos;

    src_pos = strstr(path, "src\\");
    if (src_pos) return src_pos;

    src_pos = strstr(path, "src/");
    if (src_pos) return src_pos;

    const char* slash = strrchr(path, '/');
    const char* backslash = strrchr(path, '\\');
    const char* last = (slash > backslash) ? slash : backslash;

    return last ? last + 1 : path;
}

#define PIXL_LOG_IMPL(prefix, fmt, ...) \
    do { \
        char buffer[1024]; \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
        fprintf(stdout, "[%s][%s:%d][%s] %s\n", \
                prefix, \
                pixlShortFile(__FILE__), \
                __LINE__, \
                __func__, \
                buffer); \
    } while (0)

#define LOG(fmt, ...)    PIXL_LOG_IMPL("LOG", fmt, ##__VA_ARGS__)
#define WARN(fmt, ...)   PIXL_LOG_IMPL("WRN", fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...)  PIXL_LOG_IMPL("ERR", fmt, ##__VA_ARGS__)

#define ASSERT(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            char buffer[1024]; \
            snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
            fprintf(stderr, "[ASSERT][%s:%d] FAILED (%s) : %s\n", \
                    pixlShortFile(__FILE__), \
                    __LINE__, \
                    #expr, \
                    buffer); \
            abort(); \
        } \
    } while (0)

#endif /* LOG_H */