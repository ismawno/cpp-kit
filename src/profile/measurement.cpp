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
    duration_per_call = duration_over_calls / parent_relative_calls;
    total_calls = parent_relative_calls * parent.total_calls;

    const float parent_per_call = parent.duration_per_call.as<kit::time::nanoseconds, float>();
    parent_relative_percent =
        parent_per_call > 0.f ? duration_over_calls.as<kit::time::nanoseconds, float>() / parent_per_call : 0.f;

    total_percent = parent_relative_percent * parent.total_percent;
    for (measurement &m : children)
        m.compute_relative_measurements(*this);
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

    if (measure.children.size() != children.size())
        return;

    for (std::size_t i = 0; i < children.size(); i++)
    {
        KIT_ASSERT_WARN(children[i].name == measure.children[i].name, "Children in both measures have different names")
        children[i].smooth_measurements(measure.children[i], smoothness);
    }
}

const measurement *measurement::child(const char *name) const
{
    for (const measurement &m : children)
        if (m.name == name)
            return &m;
    return nullptr;
}

measurement *measurement::child(const char *name)
{
    for (measurement &m : children)
        if (m.name == name)
            return &m;
    return nullptr;
}
} // namespace kit