#pragma once

#ifdef KIT_PROFILE

#include "kit/profiling/instrumentor.hpp"

#ifndef KIT_AKNOWLEDGE_PROFILE_THREAD_UNSAFETY
#pragma message(                                                                                                       \
        "Profiling tools provided by cpp-kit are THREAD UNSAFE. Be careful when using them in multi-threaded applications. Any KIT_PERF macro present in multi-threaded code will crash. Define the KIT_AKNOWLEDGE_PROFILE_THREAD_UNSAFETY macro to silence this warning")
#endif

#define KIT_PERF_SESSION(name) kit::perf::instrumentor::scoped_session ss##__LINE__(name);
#define KIT_PERF_SCOPE(name) kit::perf::instrumentor::scoped_measurement sm##__LINE__(name);
#define KIT_PERF_FUNCTION() kit::perf::instrumentor::scoped_measurement sm##__LINE__(__FUNCTION__);
#ifdef __PRETTY_FUNCTION__
#define KIT_PERF_PRETTY_FUNCTION() kit::perf::instrumentor::scoped_measurement sm##__LINE__(__PRETTY_FUNCTION__);
#else
#define KIT_PERF_PRETTY_FUNCTION() KIT_PERF_FUNCTION()
#endif
#else
#define KIT_PERF_SESSION(name)
#define KIT_PERF_SCOPE(name)
#define KIT_PERF_FUNCTION()
#define KIT_PERF_PRETTY_FUNCTION()
#endif
