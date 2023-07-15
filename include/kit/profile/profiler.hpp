#ifndef KIT_PROFILER_HPP
#define KIT_PROFILER_HPP

#include "kit/profile/profile_stats.hpp"
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
    long long start, end;
};

class profiler
{
  public:
    enum profile_export
    {
        FILE = 0x01,
        HIERARCHY = 0x02
    };

    static profiler &get();

    void begin_session(std::uint8_t pexport, const char *name = "results");
    void begin_timer();
    void end_timer(const profile_result &result);
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
    std::uint32_t m_count = 0, m_runs = 0, m_max_mb = 200;
    const char *m_session, *m_extension = ".json", *m_path = "./";
    std::uint8_t m_export;
    std::unordered_map<std::string, profile_stats> m_hierarchy;
    std::stack<profile_stats> m_current_hierarchy;
    float m_smoothness = 0.f;

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