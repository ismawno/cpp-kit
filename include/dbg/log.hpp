#ifndef DBG_LOG_HPP
#define DBG_LOG_HPP

#ifdef DEBUG
#include <signal.h>
#include <spdlog/spdlog.h>

#ifdef __clang__
#define DBG_BREAK() __builtin_debugtrap();
#elif defined(_MSVC_VER)
#include <intrin.h>
#define DBG_BREAK() __debugbreak();
#elif defined(SIGTRAP)
#define DBG_BREAK() raise(SIGTRAP);
#elif defined(SIGABRT)
#define DBG_BREAK() raise(SIGABRT);
#endif

#define DBG_SET_LEVEL(lvl) spdlog::set_level(spdlog::level::lvl);
#define DBG_SET_PATTERN(patt) spdlog::set_pattern(patt);

#define DBG_TRACE(...) spdlog::trace(__VA_ARGS__);
#define DBG_DEBUG(...) spdlog::debug(__VA_ARGS__);
#define DBG_INFO(...) spdlog::info(__VA_ARGS__);
#define DBG_WARN(...) spdlog::warn(__VA_ARGS__);
#define DBG_ERROR(...)                                                                                                 \
    spdlog::error(__VA_ARGS__);                                                                                        \
    DBG_BREAK()
#define DBG_CRITICAL(...)                                                                                              \
    spdlog::critical(__VA_ARGS__);                                                                                     \
    DBG_BREAK()

#define DBG_ASSERT_TRACE(cond, ...)                                                                                    \
    if (!(cond))                                                                                                       \
        spdlog::trace(__VA_ARGS__);
#define DBG_ASSERT_DEBUG(cond, ...)                                                                                    \
    if (!(cond))                                                                                                       \
        spdlog::debug(__VA_ARGS__);
#define DBG_ASSERT_INFO(cond, ...)                                                                                     \
    if (!(cond))                                                                                                       \
        spdlog::info(__VA_ARGS__);
#define DBG_ASSERT_WARN(cond, ...)                                                                                     \
    if (!(cond))                                                                                                       \
        spdlog::warn(__VA_ARGS__);
#define DBG_ASSERT_ERROR(cond, ...)                                                                                    \
    if (!(cond))                                                                                                       \
    {                                                                                                                  \
        spdlog::error(__VA_ARGS__);                                                                                    \
        DBG_BREAK()                                                                                                    \
    }
#define DBG_ASSERT_CRITICAL(cond, ...)                                                                                 \
    if (!(cond))                                                                                                       \
    {                                                                                                                  \
        spdlog::critical(__VA_ARGS__);                                                                                 \
        DBG_BREAK()                                                                                                    \
    }

#define DBG_CHECK_RETURN_VALUE(expression, expected, level, ...) DBG_ASSERT_##level(expression == expected, __VA_ARGS__)
#else
#define DBG_SET_LEVEL(lvl)
#define DBG_SET_PATTERN(patt)

#define DBG_TRACE(...)
#define DBG_DEBUG(...)
#define DBG_INFO(...)
#define DBG_WARN(...)
#define DBG_ERROR(...)
#define DBG_CRITICAL(...)
#define DBG_FATAL(...)

#define DBG_ASSERT_TRACE(cond, ...)
#define DBG_ASSERT_DEBUG(cond, ...)
#define DBG_ASSERT_INFO(cond, ...)
#define DBG_ASSERT_WARN(cond, ...)
#define DBG_ASSERT_ERROR(cond, ...)
#define DBG_ASSERT_CRITICAL(cond, ...)
#define DBG_ASSERT_FATAL(cond, ...)

#define DBG_CHECK_RETURN_VALUE(expression, expected, level, msg) expression;
#endif
#endif