#ifndef KIT_PERF_HPP
#define KIT_PERF_HPP

#include "kit/profile/profiler.hpp"

#ifdef KIT_PROFILE
#define KIT_PERF_BEGIN_SESSION(name, pexport) kit::profiler::get().begin_session(pexport, name);
#define KIT_PERF_END_SESSION() kit::profiler::get().end_session();
#define KIT_PERF_SCOPE(name) kit::profiler::ptimer tm##__LINE__(name);
#define KIT_PERF_FUNCTION() kit::profiler::ptimer tm##__LINE__(__FUNCTION__);
#ifdef __PRETTY_FUNCTION__
#define KIT_PERF_PRETTY_FUNCTION() kit::profiler::ptimer tm##__LINE__(__PRETTY_FUNCTION__);
#else
#define KIT_PERF_PRETTY_FUNCTION() KIT_PERF_FUNCTION()
#endif
#define KIT_PERF_SET_MAX_FILE_MB(size) kit::profiler::get().max_mb(size);
#define KIT_PERF_SET_EXTENSION(ext) kit::profiler::get().extension(ext);
#define KIT_PERF_SET_PATH(pth) kit::profiler::get().path(pth);
#else
#define KIT_PERF_BEGIN_SESSION(pexport, name)
#define KIT_PERF_END_SESSION()
#define KIT_PERF_SCOPE(name)
#define KIT_PERF_FUNCTION()
#define KIT_PERF_PRETTY_FUNCTION()
#define KIT_PERF_SET_MAX_FILE_MB(size)
#define KIT_PERF_SET_EXTENSION(extension)
#define KIT_PERF_SET_PATH(path)
#endif

#endif