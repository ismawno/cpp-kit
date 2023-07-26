#include "kit/internal/pch.hpp"
#include "kit/profile/instrumentor.hpp"

namespace kit
{
instrumentor::timer::timer(const char *name) : nameable(name)
{
    KIT_ASSERT_ERROR(name, "Timer name cannot be null")
    instrumentor::get().begin_measurement(name);
}

instrumentor::timer::~timer()
{
    instrumentor::get().end_measurement(name(), m_clock.start_time(), m_clock.current_time(), m_clock.elapsed());
}

instrumentor &instrumentor::get()
{
    static instrumentor inst;
    return inst;
}

void instrumentor::begin_session(const char *name, const std::uint8_t format)
{
    KIT_ASSERT_ERROR(name, "Session name must not be null")
    KIT_ASSERT_ERROR(!m_session_name, "There is already a session running")
    KIT_ASSERT_ERROR(format & output_format::JSON_TRACE | format & output_format::HIERARCHY, "Must be a valid format")

    if (format & output_format::JSON_TRACE)
        open_file();
}

void instrumentor::end_session()
{
    KIT_ASSERT_ERROR(m_session_name, "No session is currently running")
    if (m_format & output_format::JSON_TRACE)
    {
        close_file();
        m_file_count = 0;
    }
}

void instrumentor::begin_measurement(const char *name)
{
    KIT_ASSERT_ERROR(m_session_name, "A session must be active to profile")
    m_current_measurements.emplace(name);
}

void instrumentor::end_measurement(const char *name, long long start, long long end, time duration)
{
    if (m_format & output_format::JSON_TRACE)
        write_measurement(name, start, end);
    if (!(m_format & output_format::HIERARCHY))
        return;

    measurement measure = std::move(m_current_measurements.top());
    m_current_measurements.pop();
    if (m_current_measurements.empty())
    {
        measure.duration_over_calls = duration;
        m_head_measurement = std::move(measure);
        return;
    }

    measurement &parent = m_current_measurements.top();
    measurement *equivalent = parent.child(name);

    if (!equivalent)
    {
        parent.children.push_back(std::move(measure));
        return;
    }

    equivalent->duration_over_calls += duration;
    equivalent->parent_relative_calls++;
    equivalent->children = std::move(measure.children);
}

const measurement &instrumentor::last_measurement() const
{
    return m_head_measurement;
}

float instrumentor::measurement_smoothness() const
{
    return m_smoothness;
}
void instrumentor::measurement_smoothness(const float smoothness)
{
    KIT_ASSERT_ERROR(smoothness >= 0.f && smoothness < 1.f, "Smoothness must be in the range [0, 1)")
    m_smoothness = smoothness;
}

void instrumentor::open_file()
{
    if (!std::filesystem::exists(directory_path))
        std::filesystem::create_directories(directory_path);

    m_file_count++;
    m_output.open(directory_path + std::string(m_session_name) + "-" + std::to_string(m_file_count) + ".json");
    write_header();
}
void instrumentor::close_file()
{
    write_footer();
    m_output.close();
}

void instrumentor::write_measurement(const char *name, long long start, long long end)
{
    KIT_ASSERT_ERROR(m_session_name, "A session must be active to write a measurement")
    KIT_ASSERT_ERROR(m_output.is_open(), "File must be open to write a measurement")

    if (m_output.tellp() > max_mb_per_file * 1000000)
    {
        close_file();
        open_file();
    }
    else
        m_output << ",\n";

    std::string new_name = name;
    std::replace(new_name.begin(), new_name.end(), '"', '\'');

    m_output << "\t\t{";
    m_output << "\"cat\":\"function\",";
    m_output << "\"dur\":" << end - start << ",";
    m_output << "\"name\":\"" << name << "\",";
    m_output << "\"ph\":\"X\",";
    m_output << "\"pid\":0,";
    m_output << "\"tid\":0,";
    m_output << "\"ts\":" << start;
    m_output << "}";
}

void instrumentor::write_header()
{
    m_output << "{\n\t\"otherData\": {},\"traceEvents\":\n\t[\n";
}
void instrumentor::write_footer()
{
    m_output << "\n\t]\n}\n";
}
} // namespace kit