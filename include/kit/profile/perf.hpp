#ifndef PERF_HPP
#define PERF_HPP

#include "perf/timer.hpp"
#include "perf/profiler.hpp"

#ifdef PERF
#define PERF_BEGIN_SESSION(name, pexport) perf::profiler::get().begin_session(pexport, name);
#define PERF_END_SESSION() perf::profiler::get().end_session();
#define PERF_SCOPE(name) perf::timer tm##__LINE__(name);
#define PERF_FUNCTION() perf::timer tm##__LINE__(__FUNCTION__);
#ifdef __PRETTY_FUNCTION__
#define PERF_PRETTY_FUNCTION() perf::timer tm##__LINE__(__PRETTY_FUNCTION__);
#else
#define PERF_PRETTY_FUNCTION() PERF_FUNCTION()
#endif
#define PERF_SET_MAX_FILE_MB(size) perf::profiler::get().max_mb(size);
#define PERF_SET_EXTENSION(ext) perf::profiler::get().extension(ext);
#define PERF_SET_PATH(pth) perf::profiler::get().path(pth);
#else
#define PERF_BEGIN_SESSION(pexport, name)
#define PERF_END_SESSION()
#define PERF_SCOPE(name)
#define PERF_FUNCTION()
#define PERF_PRETTY_FUNCTION()
#define PERF_SET_MAX_FILE_MB(size)
#define PERF_SET_EXTENSION(extension)
#define PERF_SET_PATH(path)
#endif

#endif