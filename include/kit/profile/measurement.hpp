#ifndef KIT_MEASUREMENT_HPP
#define KIT_MEASUREMENT_HPP

#include "kit/interface/nameable.hpp"
#include "kit/profile/time.hpp"

namespace kit
{
class measurement : public nameable
{
  public:
    measurement(const char *name);

    time duration_per_call;
    time duration_over_calls;

    std::uint32_t parent_relative_calls = 1;
    std::uint32_t total_calls = 1;

    float parent_relative_percent = 1.f;
    float total_percent = 1.f;

    std::vector<measurement> children;

    void compute_relative_measurements();
    void smooth_measurements(const measurement &measure, float smoothness);

    const measurement *child(const char *name) const;
    measurement *child(const char *name);

  private:
    void compute_relative_measurements(const measurement &parent);
};

} // namespace kit

#endif