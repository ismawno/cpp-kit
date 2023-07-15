#ifndef MEM_CORE_HPP
#define MEM_CORE_HPP

#if !defined(HAS_DEBUG_LOG_TOOLS) && !defined(HAS_DEBUG_MACROS)
#define HAS_DEBUG_MACROS
#define DBG_SET_LEVEL(...)
#define DBG_SET_PATTERN(...)

#define DBG_TRACE(...)
#define DBG_DEBUG(...)
#define DBG_INFO(...)
#define DBG_WARN(...)
#define DBG_ERROR(...)
#define DBG_CRITICAL(...)
#define DBG_FATAL(...)

#define DBG_ASSERT_TRACE(...)
#define DBG_ASSERT_DEBUG(...)
#define DBG_ASSERT_INFO(...)
#define DBG_ASSERT_WARN(...)
#define DBG_ASSERT_ERROR(...)
#define DBG_ASSERT_CRITICAL(...)
#define DBG_ASSERT_FATAL(...)
#endif

#endif