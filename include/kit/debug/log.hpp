#ifndef KIT_LOG_HPP
#define KIT_LOG_HPP

#if defined(KIT_LOG) && defined(KIT_USE_SPDLOG)
#include <signal.h>
#include <spdlog/spdlog.h>

#ifdef __clang__
#define KIT_BREAK() __builtin_debugtrap();
#elif defined(_MSVC_VER)
#include <intrin.h>
#define KIT_BREAK() __debugbreak();
#elif defined(SIGTRAP)
#define KIT_BREAK() raise(SIGTRAP);
#elif defined(SIGABRT)
#define KIT_BREAK() raise(SIGABRT);
#endif

#define KIT_SET_LEVEL(lvl) spdlog::set_level(spdlog::level::lvl);
#define KIT_SET_PATTERN(patt) spdlog::set_pattern(patt);

#define KIT_TRACE(...) spdlog::trace(__VA_ARGS__);
#define KIT_DEBUG(...) spdlog::debug(__VA_ARGS__);
#define KIT_INFO(...) spdlog::info(__VA_ARGS__);
#define KIT_WARN(...) spdlog::warn(__VA_ARGS__);
#define KIT_ERROR(...)                                                                                                 \
    spdlog::error(__VA_ARGS__);                                                                                        \
    KIT_BREAK()
#define KIT_CRITICAL(...)                                                                                              \
    spdlog::critical(__VA_ARGS__);                                                                                     \
    KIT_BREAK()

#define KIT_ASSERT_TRACE(cond, ...)                                                                                    \
    if (!(cond))                                                                                                       \
        spdlog::trace(__VA_ARGS__);
#define KIT_ASSERT_DEBUG(cond, ...)                                                                                    \
    if (!(cond))                                                                                                       \
        spdlog::debug(__VA_ARGS__);
#define KIT_ASSERT_INFO(cond, ...)                                                                                     \
    if (!(cond))                                                                                                       \
        spdlog::info(__VA_ARGS__);
#define KIT_ASSERT_WARN(cond, ...)                                                                                     \
    if (!(cond))                                                                                                       \
        spdlog::warn(__VA_ARGS__);
#define KIT_ASSERT_ERROR(cond, ...)                                                                                    \
    if (!(cond))                                                                                                       \
    {                                                                                                                  \
        spdlog::error(__VA_ARGS__);                                                                                    \
        KIT_BREAK()                                                                                                    \
    }
#define KIT_ASSERT_CRITICAL(cond, ...)                                                                                 \
    if (!(cond))                                                                                                       \
    {                                                                                                                  \
        spdlog::critical(__VA_ARGS__);                                                                                 \
        KIT_BREAK()                                                                                                    \
    }

#define KIT_CHECK_RETURN_VALUE(expression, expected, level, ...) KIT_ASSERT_##level(expression == expected, __VA_ARGS__)
#else
#define KIT_SET_LEVEL(lvl)
#define KIT_SET_PATTERN(patt)

#define KIT_TRACE(...)
#define KIT_DEBUG(...)
#define KIT_INFO(...)
#define KIT_WARN(...)
#define KIT_ERROR(...)
#define KIT_CRITICAL(...)
#define KIT_FATAL(...)

#define KIT_ASSERT_TRACE(cond, ...)
#define KIT_ASSERT_DEBUG(cond, ...)
#define KIT_ASSERT_INFO(cond, ...)
#define KIT_ASSERT_WARN(cond, ...)
#define KIT_ASSERT_ERROR(cond, ...)
#define KIT_ASSERT_CRITICAL(cond, ...)
#define KIT_ASSERT_FATAL(cond, ...)

#define KIT_CHECK_RETURN_VALUE(expression, expected, level, ...) expression;
#endif
#endif