#ifndef KIT_PROFILER_HPP
#define KIT_PROFILER_HPP

#include "kit/profile/profile_stats.hpp"
#include "kit/profile/clock.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <stack>

namespace kit
{
struct profile_result
{
    const char *name;
    long long start;
    long long end;
};

class profiler
{
  public:
    enum class output
    {
        FILE = 0x01,
        HIERARCHY = 0x02
    };
    class ptimer : public nameable
    {
      public:
        explicit ptimer(const char *name);
        ~ptimer();

      private:
        clock m_clock;
    };

    static profiler &get();

    void begin_session(std::uint8_t poutput, const char *name = "results");
    void end_session();
    void write(const profile_result &result);

    const std::unordered_map<std::string, profile_stats> &hierarchy() const;

    std::uint32_t max_mb() const;
    void max_mb(std::uint32_t size);

    const char *extension() const;
    void extension(const char *extension);

    const char *path() const;
    void path(const char *path);

    float smoothness() const;
    void smoothness(float smoothness);

  private:
    profiler() = default;

    std::ofstream m_output;
    std::uint32_t m_count = 0;
    std::uint32_t m_runs = 0;
    std::uint32_t m_max_mb = 200;

    const char *m_session;
    const char *m_extension = ".json";
    const char *m_path = "./";

    std::uint8_t m_output_format;
    std::unordered_map<std::string, profile_stats> m_hierarchy;
    std::stack<profile_stats> m_current_hierarchy;
    float m_smoothness = 0.f;

    void begin_timer();
    void end_timer(const profile_result &result);

    void end_hierarchy(profile_stats &head);
    void add_to_hierarchy(const profile_result &result);

    void open_file();
    void close_file();

    void write_header();
    void write_footer();

    profiler(const profiler &) = delete;
};

} // namespace kit
#endif