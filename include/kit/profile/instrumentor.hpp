#ifndef KIT_INSTRUMENTOR_HPP
#define KIT_INSTRUMENTOR_HPP

#include "kit/interface/nameable.hpp"
#include "kit/profile/time.hpp"
#include "kit/profile/clock.hpp"
#include "kit/profile/measurement.hpp"
#include <stack>
#include <fstream>

namespace kit
{
class instrumentor
{
  public:
    class timer : public nameable
    {
      public:
        explicit timer(const char *name);
        ~timer();

      private:
        clock m_clock;
    };

    struct output_format
    {
        inline static constexpr std::uint8_t JSON_TRACE = 1 << 0;
        inline static constexpr std::uint8_t HIERARCHY = 1 << 1;
    };

    static inline const char *directory_path = "profile-results/";
    static inline std::uint32_t max_mb_per_file = 200;

    static void begin_session(const char *name, std::uint8_t format = output_format::JSON_TRACE);
    static void end_session();

    static bool has_hierarchy_measurement(const char *session);
    static const measurement &hierarchy_measurement(const char *session);
    static float measurement_smoothness();
    static void measurement_smoothness(float smoothness);

  private:
    instrumentor() = delete;

    static void begin_measurement(const char *name);
    static void end_measurement(const char *name, long long start, long long end, time duration);

    static void open_file();
    static void close_file();

    static void write_header();
    static void write_measurement(const char *name, long long start, long long end);
    static void write_footer();

    static inline std::stack<measurement> s_current_measurements{};
    static inline std::unordered_map<const char *, measurement> s_head_measurements{};

    static inline const char *s_session_name = nullptr;

    static inline std::uint8_t s_format;
    static inline std::ofstream s_output;
    static inline std::uint32_t s_file_count = 0;

    static inline float s_smoothness = 0.f;
};
} // namespace kit

#endif