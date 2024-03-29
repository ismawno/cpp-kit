#pragma once

#include "kit/profiling/time.hpp"

namespace kit::perf
{
class clock
{
  public:
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
    clock();

    long long start_time() const;
    long long current_time() const;

    time elapsed() const;
    time restart();

  private:
    time_point m_start;
};
} // namespace kit::perf
