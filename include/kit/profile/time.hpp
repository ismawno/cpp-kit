#ifndef KIT_TIME_HPP
#define KIT_TIME_HPP

#include <chrono>

namespace kit
{
class time
{
  public:
    using nanoseconds = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;

    explicit time(nanoseconds elapsed = nanoseconds::zero());

    template <typename TimeUnit, typename T> T as() const
    {
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>,
                      "Type T must be either a floating point or an integer type");
        if constexpr (std::is_floating_point_v<T>)
            return std::chrono::duration<T, typename TimeUnit::period>(m_elapsed).count();
        else
        {
            const auto as_time_unit = std::chrono::duration_cast<TimeUnit>(m_elapsed);
            return std::chrono::duration<T, typename TimeUnit::period>(as_time_unit).count();
        }
    }

    static void sleep(time tm);

    template <typename TimeUnit, typename T> static time from(T elapsed)
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

    template <typename T> time operator*(const T scalar) const
    {
        return time(std::chrono::round<nanoseconds>(m_elapsed * scalar));
    }
    template <typename T> time operator/(const T scalar) const
    {
        return time(std::chrono::round<nanoseconds>(m_elapsed / scalar));
    }

    template <typename T> time &operator*=(const T scalar)
    {
        m_elapsed *= scalar;
        return *this;
    }
    template <typename T> time &operator/=(const T scalar)
    {
        m_elapsed /= scalar;
        return *this;
    }

  private:
    nanoseconds m_elapsed;
};

template <typename T> time operator*(const T scalar, const time &rhs)
{
    return rhs * scalar;
}
} // namespace kit

#endif