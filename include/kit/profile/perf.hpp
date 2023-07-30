#ifndef KIT_PERF_HPP
#define KIT_PERF_HPP

#include "kit/profile/instrumentor.hpp"

#ifdef KIT_PROFILE
#define KIT_PERF_BEGIN_SESSION(name, export_format) kit::instrumentor::begin_session(name, export_format);
#define KIT_PERF_END_SESSION() kit::instrumentor::end_session();
#define KIT_PERF_SCOPE(name) kit::instrumentor::timer tm##__LINE__(name);
#define KIT_PERF_FUNCTION() kit::instrumentor::timer tm##__LINE__(__FUNCTION__);
#ifdef __PRETTY_FUNCTION__
#define KIT_PERF_PRETTY_FUNCTION() kit::instrumentor::timer tm##__LINE__(__PRETTY_FUNCTION__);
#else
#define KIT_PERF_PRETTY_FUNCTION() KIT_PERF_FUNCTION()
#endif
#define KIT_PERF_SET_MAX_FILE_MB(size) kit::instrumentor::max_mb_per_file = size;
#define KIT_PERF_SET_DIRECTORY_PATH(pth) kit::instrumentor::directory_path = pth;
#else
#define KIT_PERF_BEGIN_SESSION(export_format, name)
#define KIT_PERF_END_SESSION()
#define KIT_PERF_SCOPE(name)
#define KIT_PERF_FUNCTION()
#define KIT_PERF_PRETTY_FUNCTION()
#define KIT_PERF_SET_MAX_FILE_MB(size)
#define KIT_PERF_SET_DIRECTORY_PATH(pth)
#endif

#endif