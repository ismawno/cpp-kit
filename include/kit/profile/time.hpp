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

    template <typename Type, typename TimeUnit> Type as() const
    {
        return std::chrono::duration<Type, TimeUnit>(m_elapsed).count();
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
    time(std::chrono::nanoseconds elapsed);
    std::chrono::nanoseconds m_elapsed;

    friend class clock;
};
} // namespace kit

#endif