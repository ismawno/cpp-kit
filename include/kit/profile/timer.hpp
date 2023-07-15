#ifndef KIT_TIMER_HPP
#define KIT_TIMER_HPP

#include <fstream>
#include <chrono>

namespace kit
{
class timer
{
  public:
    timer(const char *m_name);
    ~timer();

    long long stop();

  private:
    bool m_stopped = false;
    const std::chrono::time_point<std::chrono::high_resolution_clock> m_startpoint;
    const char *m_name;
};
} // namespace kit

#endif