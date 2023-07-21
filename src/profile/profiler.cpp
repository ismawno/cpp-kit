#include "kit/internal/pch.hpp"
#include "kit/profile/profiler.hpp"

namespace kit
{
profiler &profiler::get()
{
    static profiler p;
    return p;
}

void profiler::begin_session(std::uint8_t poutput, const char *name)
{
    m_session = name;
    m_output_format = poutput;
    if (m_output_format & (std::uint8_t)output::FILE)
        open_file();
    if (m_output_format & (std::uint8_t)output::HIERARCHY)
    {
        KIT_ASSERT_ERROR(m_current_hierarchy.empty(),
                         "Starting a new profile session with an unfinished hierarchy from the last session. Number of "
                         "pending parents: {0}.",
                         m_current_hierarchy.size())
    }
}

void profiler::begin_timer()
{
    if (m_output_format & (std::uint8_t)output::HIERARCHY)
        m_current_hierarchy.emplace();
}
void profiler::end_timer(const profile_result &result)
{
    if (m_output_format & (std::uint8_t)output::FILE)
        write(result);
    if (m_output_format & (std::uint8_t)output::HIERARCHY)
        add_to_hierarchy(result);
}

void profiler::end_session()
{
    if (m_output_format & (std::uint8_t)output::FILE)
    {
        close_file();
        m_runs = 0;
    }
    KIT_ASSERT_ERROR(m_current_hierarchy.empty(),
                     "Ending profile session with pending records. Number of pending records: {0}.",
                     m_current_hierarchy.size())
}

void profiler::write(const profile_result &result)
{
    KIT_ASSERT_ERROR(m_output.is_open(),
                     "Trying to profile with an unfinished hierarchy. Number of pending parents: {0}.",
                     m_current_hierarchy.size())
    if (m_output.tellp() > m_max_mb * 1000000)
    {
        close_file();
        open_file();
    }
    if (m_count++ > 0)
        m_output << ",\n";

    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_output << "\t\t{";
    m_output << "\"cat\":\"function\",";
    m_output << "\"dur\":" << result.end - result.start << ",";
    m_output << "\"name\":\"" << name << "\",";
    m_output << "\"ph\":\"X\",";
    m_output << "\"pid\":0,";
    m_output << "\"tid\":0,";
    m_output << "\"ts\":" << result.start;
    m_output << "}";
}

void profiler::end_hierarchy(profile_stats &head)
{
    head.compute_relative_durations();
    if (m_smoothness != 0.f)
        head.smooth_stats(m_hierarchy.at(m_session), m_smoothness);
    m_hierarchy[m_session] = head;
}

void profiler::add_to_hierarchy(const profile_result &result)
{
    profile_stats stats = m_current_hierarchy.top();
    const long long duration = result.end - result.start;
    stats.name(result.name);

    stats.m_relative_calls = 1;
    stats.m_duration_per_call = duration;
    stats.m_duration_over_calls = duration;

    m_current_hierarchy.pop();
    if (m_current_hierarchy.empty())
    {
        end_hierarchy(stats);
        return;
    }
    profile_stats &parent = m_current_hierarchy.top();

    if (parent.m_children.find(result.name) != parent.m_children.end())
    {
        const profile_stats &last = parent.m_children.at(result.name);
        const long long last_per_call = last.m_duration_per_call;
        const long long last_over_calls = last.m_duration_over_calls;

        stats.m_relative_calls += parent.m_children.at(result.name).m_relative_calls;
        stats.m_duration_per_call = ((stats.m_relative_calls - 1) * last_per_call + duration) / stats.m_relative_calls;
        stats.m_duration_over_calls += last_over_calls;
    }

    parent.m_children[result.name] = stats;
}

void profiler::open_file()
{
    if (!std::filesystem::exists(m_path))
        std::filesystem::create_directories(m_path);
    m_output.open(m_path + std::string(m_session) + "-" + std::to_string(++m_runs) + m_extension);
    write_header();
}

void profiler::close_file()
{
    write_footer();
    m_output.close();
    m_count = 0;
}

void profiler::write_header()
{
    m_output << "{\n\t\"otherData\": {},\"traceEvents\":\n\t[\n";
}

void profiler::write_footer()
{
    m_output << "\n\t]\n}\n";
}

const std::unordered_map<std::string, profile_stats> &profiler::hierarchy() const
{
    return m_hierarchy;
}

std::uint32_t profiler::max_mb() const
{
    return m_max_mb;
}
void profiler::max_mb(const std::uint32_t size)
{
    m_max_mb = size;
}

const char *profiler::extension() const
{
    return m_extension;
}
void profiler::extension(const char *extension)
{
    m_extension = extension;
}

const char *profiler::path() const
{
    return m_path;
}
void profiler::path(const char *path)
{
    m_path = path;
}

float profiler::smoothness() const
{
    return m_smoothness;
}
void profiler::smoothness(const float smoothness)
{
    m_smoothness = smoothness;
}

profiler::ptimer::ptimer(const char *name) : nameable(name)
{
    get().begin_timer();
}
profiler::ptimer::~ptimer()
{
    get().end_timer({name(), m_clock.start_time(), m_clock.current_time()});
}
} // namespace kit