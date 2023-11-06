#include "kit/internal/pch.hpp"
#include "kit/profile/instrumentor.hpp"
#include "kit/utility/utils.hpp"

namespace kit
{
instrumentor::timer::timer(const char *name) : nameable(name)
{
    KIT_ASSERT_ERROR(name, "Timer name cannot be null")
    instrumentor::begin_measurement(name);
}

instrumentor::timer::~timer()
{
    instrumentor::end_measurement(name, m_clock.start_time(), m_clock.current_time(), m_clock.elapsed());
}

void instrumentor::begin_session(const char *name, const std::uint8_t format)
{
    KIT_ASSERT_ERROR(name, "Session name must not be null")
    KIT_ASSERT_ERROR(!s_session_name, "There is already a session running")
    KIT_ASSERT_ERROR(format & output_format::JSON_TRACE | format & output_format::HIERARCHY, "Must be a valid format")

    s_session_name = name;
    s_format = format;

    if (format & output_format::JSON_TRACE)
        open_file();
}

void instrumentor::end_session()
{
    KIT_ASSERT_ERROR(s_session_name, "No session is currently running")
    if (s_format & output_format::JSON_TRACE)
    {
        close_file();
        s_file_count = 0;
    }
    s_session_name = nullptr;
}

void instrumentor::begin_measurement(const char *name)
{
    KIT_ASSERT_ERROR(s_session_name, "A session must be active to begin a measurement")
    s_current_measurements.emplace(name);
}

void instrumentor::end_measurement(const char *name, const long long start, const long long end, const time duration)
{
    KIT_ASSERT_ERROR(s_session_name, "A session must be active to end a measurement")
    KIT_ASSERT_ERROR(!s_current_measurements.empty(), "Must first begin a measurement")

    if (s_format & output_format::JSON_TRACE)
        write_measurement(name, start, end);
    if (!(s_format & output_format::HIERARCHY))
        return;

    measurement measure = std::move(s_current_measurements.top());
    measure.duration_over_calls = duration;

    s_current_measurements.pop();
    if (s_current_measurements.empty())
    {
        measure.compute_relative_measurements();
        if (!approaches_zero(s_smoothness) && has_hierarchy_measurement(s_session_name))
            measure.smooth_measurements(s_head_measurements.at(s_session_name), s_smoothness);

        s_head_measurements[s_session_name] = std::move(measure);
        return;
    }

    measurement &parent = s_current_measurements.top();
    if (parent.children.find(name) == parent.children.end())
    {
        parent.children.emplace(measure.name, std::move(measure));
        return;
    }

    measurement &equivalent = parent.children.at(name);
    equivalent.parent_relative_calls++;
    equivalent.absorb(measure);
}

bool instrumentor::has_hierarchy_measurement(const char *session)
{
    return s_head_measurements.find(session) != s_head_measurements.end();
}

const measurement &instrumentor::hierarchy_measurement(const char *session)
{
    KIT_ASSERT_ERROR(has_hierarchy_measurement(session),
                     "The session %s was not found! Must begin the session with hierarchy format, or end the session "
                     "at least once before trying to retrieve the last measurement",
                     session)
    return s_head_measurements.at(session);
}

float instrumentor::measurement_smoothness()
{
    return s_smoothness;
}
void instrumentor::measurement_smoothness(const float smoothness)
{
    KIT_ASSERT_ERROR(smoothness >= 0.f && smoothness < 1.f, "Smoothness must be in the range [0, 1)")
    s_smoothness = smoothness;
}

void instrumentor::open_file()
{
    if (!std::filesystem::exists(directory_path))
        std::filesystem::create_directories(directory_path);

    s_file_count++;

    const std::string filepath =
        directory_path + std::string(s_session_name) + "-" + std::to_string(s_file_count) + ".json";
    s_output.open(filepath);
    write_header();
}
void instrumentor::close_file()
{
    write_footer();
    s_output.close();
}

void instrumentor::write_measurement(const char *name, long long start, long long end)
{
    KIT_ASSERT_ERROR(s_session_name, "A session must be active to write a measurement")
    KIT_ASSERT_ERROR(s_output.is_open(), "File must be open to write a measurement")
    static bool first_call_ever = true;

    if (s_output.tellp() > max_mb_per_file * 1000000)
    {
        close_file();
        open_file();
    }
    else if (!first_call_ever)
        s_output << ",\n";

    first_call_ever = false;

    std::string new_name = name;
    std::replace(new_name.begin(), new_name.end(), '"', '\'');

    s_output << "\t\t{";
    s_output << "\"cat\":\"function\",";
    s_output << "\"dur\":" << end - start << ",";
    s_output << "\"name\":\"" << name << "\",";
    s_output << "\"ph\":\"X\",";
    s_output << "\"pid\":0,";
    s_output << "\"tid\":0,";
    s_output << "\"ts\":" << start;
    s_output << "}";
}

void instrumentor::write_header()
{
    s_output << "{\n\t\"otherData\": {},\"traceEvents\":\n\t[\n";
}
void instrumentor::write_footer()
{
    s_output << "\n\t]\n}\n";
}
} // namespace kit