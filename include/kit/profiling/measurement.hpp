#pragma once

#include "kit/profiling/time.hpp"

namespace kit::perf
{
struct measurement
{
    const char *name;
    time average;
    time cumulative;
    float percent = 0.f;
    std::uint32_t calls = 0;
};

} // namespace kit::perf
