#ifndef KIT_INSTRUMENTOR_HPP
#define KIT_INSTRUMENTOR_HPP

#include "kit/interface/nameable.hpp"
#include "kit/profile/time.hpp"
#include "kit/profile/clock.hpp"
#include "kit/profile/measurement.hpp"

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

    enum class output_format
    {
        JSON_TRACE = 0x01,
        MEASUREMENT_HIERARCHY = 0x02
    };

    static instrumentor &get();

    const char *directory_path = "./";
    std::uint32_t max_mb_per_file = 200;

    void begin_session(const char *name, output_format format);
    void end_session();

    const measurement &last_measurement() const;
    float measurement_smoothness() const;
    void measurement_smoothness(float smoothness);

  private:
    instrumentor() = default;

    void begin_measurement(const char *name);
    void end_measurement(const char *name, long long start, long long end, time duration);

    void open_file();
    void close_file();

    void write_header();
    void write_measurement(const char *name, long long start, long long end);
    void write_footer();

    std::stack<measurement> m_current_measurements;
    measurement m_head_measurement{"$NULL$"};

    const char *m_session_name = nullptr;
    const char *m_total_measurement_name = nullptr;

    output_format m_format;
    std::ofstream m_output;
    std::uint32_t m_file_count = 0;

    float m_smoothness = 0.f;
};
} // namespace kit

#endif