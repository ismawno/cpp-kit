#pragma once

#ifdef KIT_PROFILE

#include "kit/profiling/instrumentor.hpp"

#define KIT_PERF_SCOPE(name) kit::perf::instrumentor::scoped_measurement sm##__LINE__(name);
#define KIT_PERF_FUNCTION() kit::perf::instrumentor::scoped_measurement sm##__LINE__(__func__);
#ifdef __PRETTY_FUNCTION__
#define KIT_PERF_PRETTY_FUNCTION() kit::perf::instrumentor::scoped_measurement sm##__LINE__(__PRETTY_FUNCTION__);
#else
#define KIT_PERF_PRETTY_FUNCTION() KIT_PERF_FUNCTION()
#endif
#else
#define KIT_PERF_SCOPE(name)
#define KIT_PERF_FUNCTION()
#define KIT_PERF_PRETTY_FUNCTION()
#endif
