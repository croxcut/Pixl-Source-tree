#ifndef __LOG__
#define __LOG__

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

enum Level { INFO, WARN, ERROR };

inline std::string currentTime() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000;

    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_struct;
#ifdef _WIN32
    localtime_s(&tm_struct, &now_c);
#else
    localtime_r(&now_c, &tm_struct);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}


#define LOG(lvl, fmt, ...) \
    do { \
        const char* prefix = \
            ((lvl) == INFO)  ? "INFO"  : \
            ((lvl) == WARN)  ? "WARN"  : \
            ((lvl) == ERROR) ? "ERROR" : "DEBUG"; \
        char buffer[1024]; \
        snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
        std::cout << "[PIXL][DEBUG][" << currentTime() << "] " \
                  << prefix << " : " << buffer << "\n"; \
    } while(0)

#define ASSERT(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            char buffer[1024]; \
            snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
            std::cerr << "[PIXL][ASSERT][" << currentTime() << "] ASSERTION FAILED (" << #expr << ") : " \
                      << buffer << "\n"; \
            std::abort(); \
        } \
    } while(0)

#endif