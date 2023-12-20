#pragma once

#include "kit/interface/nameable.hpp"
#include "kit/profile/time.hpp"
#include <unordered_map>

namespace kit
{
class measurement : public nameable
{
  public:
    measurement(const char *name = nullptr);

    time duration_per_call;
    time duration_over_calls;

    std::uint32_t parent_relative_calls = 1;
    std::uint32_t total_calls = 1;

    float parent_relative_percent = 1.f;
    float total_percent = 1.f;

    std::unordered_map<const char *, measurement> children;

    void compute_relative_measurements();
    void smooth_measurements(const measurement &measure, float smoothness);

    void absorb(measurement &other);

  private:
    void compute_relative_measurements(const measurement &parent);
};

} // namespace kit
