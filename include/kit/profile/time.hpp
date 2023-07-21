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

    template <typename TimeUnit, typename Type> Type as() const
    {
        return std::chrono::duration<Type, TimeUnit>(m_elapsed).count();
    }

    static void sleep(time tm);

    template <typename TimeDuration, typename Type> static time from(Type elapsed)
    {
        return time(std::chrono::round<TimeDuration>(std::chrono::duration<Type>(elapsed)));
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

  private:
    duration::nanoseconds m_elapsed;
};
} // namespace kit

#endif