#pragma once

#include "kit/utility/type_constraints.hpp"
#include <chrono>

namespace kit::perf
{
class time
{
  public:
    using nanoseconds = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;

    time(nanoseconds elapsed = nanoseconds::zero());

    template <typename TimeUnit, Numeric T> T as() const
    {
        if constexpr (std::is_floating_point_v<T>)
            return std::chrono::duration<T, typename TimeUnit::period>(m_elapsed).count();
        else
        {
            const auto as_time_unit = std::chrono::duration_cast<TimeUnit>(m_elapsed);
            return std::chrono::duration<T, typename TimeUnit::period>(as_time_unit).count();
        }
    }

    static void sleep(time tm);

    template <typename TimeUnit, Numeric T> static time from(T elapsed)
    {
        return time(std::chrono::round<nanoseconds>(std::chrono::duration<T, typename TimeUnit::period>(elapsed)));
    }

    bool operator==(const time &other) const;
    bool operator!=(const time &other) const;

    bool operator<(const time &other) const;
    bool operator>(const time &other) const;

    bool operator<=(const time &other) const;
    bool operator>=(const time &other) const;

    time operator+(const time &other) const;
    time operator-(const time &other) const;

    time &operator+=(const time &other);
    time &operator-=(const time &other);

    template <Numeric T> time operator*(const T scalar) const
    {
        return time(std::chrono::round<nanoseconds>(m_elapsed * scalar));
    }
    template <Numeric T> time operator/(const T scalar) const
    {
        return time(std::chrono::round<nanoseconds>(m_elapsed / scalar));
    }

    template <Numeric T> time &operator*=(const T scalar)
    {
        m_elapsed *= scalar;
        return *this;
    }
    template <Numeric T> time &operator/=(const T scalar)
    {
        m_elapsed /= scalar;
        return *this;
    }

  private:
    nanoseconds m_elapsed;
};

template <Numeric T> time operator*(const T scalar, const time &rhs)
{
    return rhs * scalar;
}
} // namespace kit::perf
