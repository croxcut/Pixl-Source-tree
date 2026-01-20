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

#ifndef LOG_H
#define LOG_H

#include "pxl_pre_compile.h"

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

#ifdef PIXL_LOGGER_ENABLED
    #define LOG(fmt, ...)    PIXL_LOG_IMPL("LOG", fmt, ##__VA_ARGS__)
    #define WARN(fmt, ...)   PIXL_LOG_IMPL("WRN", fmt, ##__VA_ARGS__)
    #define ERROR(fmt, ...)  PIXL_LOG_IMPL("ERR", fmt, ##__VA_ARGS__)
#else
    #define LOG(fmt, ...)    ((void)0)
    #define WARN(fmt, ...)   ((void)0)
    #define ERROR(fmt, ...)  ((void)0)
#endif

#define ASSERT(expr, fmt, ...)                                              \
    do {                                                                    \
        if (!(expr)) {                                                      \
            fprintf(stderr,                                                 \
                "[ASSERT][%s:%d][%s] FAILED (%s): " fmt "\n",               \
                pixlShortFile(__FILE__),                                    \
                __LINE__,                                                   \
                __func__,                                                   \
                #expr,                                                      \
                ##__VA_ARGS__);                                             \
            fflush(stderr);                                                 \
            abort();                                                        \
        }                                                                   \
    } while (0)

#endif /* LOG_H */