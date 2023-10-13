#include "kit/internal/pch.hpp"
#include "kit/profile/measurement.hpp"

namespace kit
{
measurement::measurement(const char *name) : nameable(name)
{
    children.reserve(16);
}

void measurement::compute_relative_measurements(const measurement &parent)
{
    // total_calls = parent_relative_calls * parent.total_calls;
    duration_per_call = duration_over_calls / total_calls;

    const float parent_over_calls = parent.duration_over_calls.as<kit::time::nanoseconds, float>();
    parent_relative_percent =
        parent_over_calls > 0.f ? duration_over_calls.as<kit::time::nanoseconds, float>() / parent_over_calls : 0.f;

    total_percent = parent_relative_percent * parent.total_percent;
    for (auto &[name, child] : children)
        child.compute_relative_measurements(*this);
}

void measurement::compute_relative_measurements()
{
    compute_relative_measurements(*this);
}

void measurement::smooth_measurements(const measurement &measure, const float smoothness)
{
    KIT_ASSERT_ERROR(smoothness >= 0.f && smoothness < 1.f, "Smoothness must be in the range [0, 1)")

    duration_per_call = smoothness * measure.duration_per_call + (1.f - smoothness) * duration_per_call;
    duration_over_calls = smoothness * measure.duration_over_calls + (1.f - smoothness) * duration_over_calls;

    parent_relative_percent =
        smoothness * measure.parent_relative_percent + (1.f - smoothness) * parent_relative_percent;
    total_percent = smoothness * measure.total_percent + (1.f - smoothness) * total_percent;

    for (auto &[name, child] : children)
        if (measure.children.find(name) != measure.children.end())
            child.smooth_measurements(measure.children.at(name), smoothness);
}

void measurement::absorb(measurement &other)
{
    duration_over_calls += other.duration_over_calls;
    total_calls += other.total_calls;
    for (auto &[name, other_child] : other.children)
        if (children.find(name) == children.end())
            children.emplace(name, std::move(other_child));
        else
            children.at(name).absorb(other_child);
}
} // namespace kit