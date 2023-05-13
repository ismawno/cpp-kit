#ifndef DBG_LOG_HPP
#define DBG_LOG_HPP

#ifdef DEBUG
#include <spdlog/spdlog.h>
#include <signal.h>

#ifdef __clang__
#define DBG_BREAK() __builtin_debugtrap();
#elif defined(SIGTRAP)
#define DBG_BREAK() raise(SIGTRAP);
#elif defined(SIGABRT)
#define DBG_BRAK() raise(SIGABRT);
#elif defined(_MSVC_VER)
#include <intrin.h>
#define DBG_BREAK() __debugbreak();
#endif

#define DBG_SET_LEVEL(lvl) spdlog::set_level(lvl);
#define DBG_SET_PATTERN(patt) spdlog::set_pattern(patt);

#define DBG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__);
#define DBG_INFO(...) SPDLOG_INFO(__VA_ARGS__);
#define DBG_WARN(...) SPDLOG_WARN(__VA_ARGS__);
#define DBG_ERROR(...)         \
    SPDLOG_ERROR(__VA_ARGS__); \
    DBG_BREAK()
#define DBG_CRITICAL(...)         \
    SPDLOG_CRITICAL(__VA_ARGS__); \
    DBG_BREAK()

#define DBG_ASSERT_TRACE(cond, ...) \
    if (!(cond))                    \
        SPDLOG_TRACE(__VA_ARGS__);
#define DBG_ASSERT_INFO(cond, ...) \
    if (!(cond))                   \
        SPDLOG_INFO(__VA_ARGS__);
#define DBG_ASSERT_WARN(cond, ...) \
    if (!(cond))                   \
        SPDLOG_WARN(__VA_ARGS__);
#define DBG_ASSERT_ERROR(cond, ...) \
    if (!(cond))                    \
    {                               \
        SPDLOG_ERROR(__VA_ARGS__);  \
        DBG_BREAK()                 \
    }
#define DBG_ASSERT_CRITICAL(cond, ...) \
    if (!(cond))                       \
    {                                  \
        SPDLOG_CRITICAL(__VA_ARGS__);  \
        DBG_BREAK()                    \
    }
#else
#define DBG_SET_PATTERN(patt)

#define DBG_TRACE(...)
#define DBG_INFO(...)
#define DBG_WARN(...)
#define DBG_ERROR(...)
#define DBG_CRITICAL(...)
#define DBG_FATAL(...)

#define DBG_ASSERT_TRACE(cond, ...)
#define DBG_ASSERT_INFO(cond, ...)
#define DBG_ASSERT_WARN(cond, ...)
#define DBG_ASSERT_ERROR(cond, ...)
#define DBG_ASSERT_CRITICAL(cond, ...)
#define DBG_ASSERT_FATAL(cond, ...)
#endif
#endif