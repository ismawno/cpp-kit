#include "kit/internal/pch.hpp"
#include "kit/profile/clock.hpp"

namespace kit
{
static clock::time_point now()
{
    return std::chrono::high_resolution_clock::now();
}
static long long point_to_long(clock::time_point point)
{
    return std::chrono::time_point_cast<std::chrono::nanoseconds>(point).time_since_epoch().count();
}

clock::clock() : m_start(now())
{
}

long long clock::start_time() const
{
    return point_to_long(m_start);
}
long long clock::current_time() const
{
    return point_to_long(now());
}

time clock::elapsed() const
{
    return time(now() - m_start);
}
time clock::restart()
{
    const time tm{time(now() - m_start)};
    m_start = now();
    return tm;
}
} // namespace kit