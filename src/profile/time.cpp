#include "kit/internal/pch.hpp"
#include "kit/profile/time.hpp"

namespace kit
{
time::time(const std::chrono::nanoseconds elapsed) : m_elapsed(elapsed)
{
}

bool time::operator==(const time &other)
{
    return m_elapsed == other.m_elapsed;
}
bool time::operator!=(const time &other)
{
    return m_elapsed != other.m_elapsed;
}

bool time::operator<(const time &other)
{
    return m_elapsed < other.m_elapsed;
}
bool time::operator>(const time &other)
{
    return m_elapsed > other.m_elapsed;
}

bool time::operator<=(const time &other)
{
    return m_elapsed <= other.m_elapsed;
}
bool time::operator>=(const time &other)
{
    return m_elapsed >= other.m_elapsed;
}

time time::operator+(const time &other)
{
    return time(m_elapsed + other.m_elapsed);
}
time time::operator-(const time &other)
{
    return time(m_elapsed - other.m_elapsed);
}

time &time::operator+=(const time &other)
{
    m_elapsed += other.m_elapsed;
    return *this;
}
time &time::operator-=(const time &other)
{
    m_elapsed -= other.m_elapsed;
    return *this;
}
} // namespace kit