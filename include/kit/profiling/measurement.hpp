#pragma once

#include "kit/interface/nameable.hpp"
#include "kit/profiling/time.hpp"
#include <unordered_map>

namespace kit::perf
{
class measurement : public nameable
{
  public:
    using nameable::nameable;

    std::size_t parent_index;
    long long start;
    long long end;

    time elapsed;
};

} // namespace kit::perf
