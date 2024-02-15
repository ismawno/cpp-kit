#pragma once

#ifdef KIT_PROFILE

#include "kit/profiling/instrumentor.hpp"

#ifndef KIT_AKNOWLEDGE_PROFILE_THREAD_UNSAFETY
#pragma message(                                                                                                       \
        "Profiling tools provided by cpp-kit are THREAD UNSAFE. Be careful when using them in multi-threaded applications. Any KIT_PERF macro present in multi-threaded code will crash. Define the KIT_AKNOWLEDGE_PROFILE_THREAD_UNSAFETY macro to silence this warning")
#endif

#define KIT_PERF_BEGIN_SESSION(name, export_format) kit::perf::instrumentor::begin_session(name, export_format);
#define KIT_PERF_END_SESSION() kit::perf::instrumentor::end_session();
#define KIT_PERF_SCOPE(name) kit::perf::instrumentor::scoped_measurement sm##__LINE__(name);
#define KIT_PERF_FUNCTION() kit::perf::instrumentor::scoped_measurement sm##__LINE__(__FUNCTION__);
#ifdef __PRETTY_FUNCTION__
#define KIT_PERF_PRETTY_FUNCTION() kit::perf::instrumentor::scoped_measurement sm##__LINE__(__PRETTY_FUNCTION__);
#else
#define KIT_PERF_PRETTY_FUNCTION() KIT_PERF_FUNCTION()
#endif
#define KIT_PERF_SET_MAX_FILE_MB(size) kit::perf::instrumentor::max_mb_per_file = size;
#define KIT_PERF_SET_DIRECTORY_PATH(pth) kit::perf::instrumentor::directory_path = pth;
#else
#define KIT_PERF_BEGIN_SESSION(export_format, name)
#define KIT_PERF_END_SESSION()
#define KIT_PERF_SCOPE(name)
#define KIT_PERF_FUNCTION()
#define KIT_PERF_PRETTY_FUNCTION()
#define KIT_PERF_SET_MAX_FILE_MB(size)
#define KIT_PERF_SET_DIRECTORY_PATH(pth)
#endif
