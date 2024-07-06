#pragma once

#include "kit/interface/nameable.hpp"
#include "kit/profiling/time.hpp"
#include <unordered_map>

namespace kit::perf
{
class measurement : public nameable<>
{
  public:
    struct metrics
    {
        time elapsed;
        float relative_percent = 0.f;
        float total_percent = 0.f;
    };

    using nameable<>::nameable;

    std::size_t parent_index;
    long long start;
    long long end;

    time elapsed;
};

} // namespace kit::perf
