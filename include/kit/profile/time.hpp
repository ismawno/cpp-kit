#ifndef KIT_TIME_HPP
#define KIT_TIME_HPP

#include <chrono>

namespace kit
{
class time
{
  public:
    using nanoseconds = std::chrono::nanoseconds::period;
    using microseconds = std::chrono::microseconds::period;
    using milliseconds = std::chrono::milliseconds::period;
    using seconds = std::chrono::seconds::period;

    struct duration
    {
        using nanoseconds = std::chrono::nanoseconds;
        using microseconds = std::chrono::microseconds;
        using milliseconds = std::chrono::milliseconds;
        using seconds = std::chrono::seconds;
    };

    time(duration::nanoseconds elapsed = duration::nanoseconds::zero());

    template <typename TimeUnit, typename T> T as() const
    {
        return std::chrono::duration<T, TimeUnit>(m_elapsed).count();
    }

    static void sleep(time tm);

    template <typename TimeUnit, typename T> static time from(T elapsed)
    {
        return time(std::chrono::round<duration::nanoseconds>(std::chrono::duration<T, TimeUnit>(elapsed)));
    }

    bool operator==(const time &other);
    bool operator!=(const time &other);

    bool operator<(const time &other);
    bool operator>(const time &other);

    bool operator<=(const time &other);
    bool operator>=(const time &other);

    time operator+(const time &other);
    time operator-(const time &other);

    time &operator+=(const time &other);
    time &operator-=(const time &other);

    template <typename T> time operator*(const T scalar)
    {
        return time(m_elapsed * scalar);
    }
    template <typename T> time operator/(const T scalar)
    {
        return time(m_elapsed / scalar);
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
    duration::nanoseconds m_elapsed;
};

template <typename T> time operator*(const T scalar, const time &rhs)
{
    return rhs * scalar;
}
} // namespace kit

#endif