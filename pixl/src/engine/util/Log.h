#ifndef __LOG__
#define __LOG__

#include <cstdarg>
#include <cstdio>
#include <iostream>

enum Level { INFO, WARN, ERROR, DEBUG};

inline void Log(Level lvl, const char* fmt, ...) {
    const char* prefix =
        (lvl == INFO) ? "INFO" :
        (lvl == WARN) ? "WARN" :
        (lvl == DEBUG) ? "DEBUG" :
                        "ERROR";

    char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::cout << "[Pixl] "  << prefix << " : " << buffer << "\n";
}

#endif