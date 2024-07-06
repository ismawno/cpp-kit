#include "kit/internal/pch.hpp"
#include "kit/profiling/instrumentor.hpp"
#include "kit/utility/utils.hpp"

namespace kit::perf
{
void instrumentor::begin_session(const char *name)
{
    KIT_ASSERT_ERROR(name, "Session name must not be null")
    KIT_ASSERT_ERROR(!s_session_name, "There is already a session running")
    s_session_name = name;
}

void instrumentor::end_session()
{
    KIT_ASSERT_ERROR(s_session_name, "No session is currently running")
    KIT_ASSERT_ERROR(s_ongoing_measurements.empty(), "Cannot end a session with ongoing measurements")
    s_session_name = nullptr;
}

void instrumentor::begin_measurement(const char *name)
{
    KIT_ASSERT_ERROR(s_session_name, "A session must be active to begin a measurement")
    KIT_ASSERT_ERROR(name, "Measurement name must not be null")
#ifdef DEBUG
    for (const char *c = name; *c; ++c)
    {
        KIT_ASSERT_ERROR(*c != '$', "The character '$' is not allowed in measurement names")
    }
#endif
    const std::string name_hash =
        s_ongoing_measurements.empty() ? name : s_ongoing_measurements.top().name_hash + "$" + std::string(name);

    if (s_ongoing_measurements.empty())
        s_head_node_names[s_session_name] = name;

    s_ongoing_measurements.emplace(ongoing_measurement{name, name_hash, clock{}});
}

void instrumentor::end_measurement()
{
    KIT_ASSERT_ERROR(s_session_name, "A session must be active to end a measurement")
    KIT_ASSERT_ERROR(!s_ongoing_measurements.empty(), "Cannot end a measurement without beginning one")

    const ongoing_measurement &ongoing = s_ongoing_measurements.top();
    const long long end = ongoing.clk.current_time();
    const long long start = ongoing.clk.start_time();
    const time elapsed = ongoing.clk.elapsed();

    measurement ms{ongoing.name};
    ms.start = start;
    ms.end = end;
    ms.elapsed = elapsed;
    const std::string name_hash = ongoing.name_hash;

    s_ongoing_measurements.pop();
    ms.parent_index = s_ongoing_measurements.empty()
                          ? SIZE_MAX
                          : s_current_measurements[s_ongoing_measurements.top().name_hash].size();
    s_current_measurements[name_hash].push_back(ms);
    if (s_ongoing_measurements.empty())
    {
        s_measurements[s_session_name] = s_current_measurements;
        s_metrics_cache[s_session_name].clear();
        s_current_measurements.clear();
    }
}

const char *instrumentor::current_session()
{
    return s_session_name;
}
bool instrumentor::has_measurements(const char *session)
{
    return s_measurements.contains(session);
}
node instrumentor::head_node(const char *session)
{
    KIT_ASSERT_ERROR(has_measurements(session),
                     "No measurements for session {0}. All measurements of a session must end for them to be available",
                     session)
    return node{s_head_node_names[session], &s_measurements[session], &s_metrics_cache[session]};
}

instrumentor::scoped_session::scoped_session(const char *name)
{
    instrumentor::begin_session(name);
}
instrumentor::scoped_session::~scoped_session()
{
    instrumentor::end_session();
}

instrumentor::scoped_measurement::scoped_measurement(const char *name)
{
    instrumentor::begin_measurement(name);
}
instrumentor::scoped_measurement::~scoped_measurement()
{
    instrumentor::end_measurement();
}

static void write_header(std::stringstream &stream)
{
    stream << "{\n\t\"otherData\": {},\"traceEvents\":\n\t[\n";
}

static void write_measurement(const measurement &ms, std::stringstream &stream)
{
    std::string name = ms.name();
    std::replace(name.begin(), name.end(), '"', '\'');

    stream << "\t\t{";
    stream << "\"cat\":\"function\",";
    stream << "\"dur\":" << ms.end - ms.start << ",";
    stream << "\"name\":\"" << name << "\",";
    stream << "\"ph\":\"X\",";
    stream << "\"pid\":0,";
    stream << "\"tid\":0,";
    stream << "\"ts\":" << ms.start;
    stream << "}";
}

static void write_footer(std::stringstream &stream)
{
    stream << "\n\t]\n}\n";
}

std::stringstream instrumentor::strstream(const char *session)
{
    std::stringstream stream;
    write_header(stream);
    const auto &measurements = s_measurements[session];
    for (const auto &[name_hash, ms] : measurements)
        for (const auto &m : ms)
            write_measurement(m, stream);
    write_footer(stream);
    return stream;
}

std::string instrumentor::str(const char *session)
{
    return strstream(session).str();
}
} // namespace kit::perf