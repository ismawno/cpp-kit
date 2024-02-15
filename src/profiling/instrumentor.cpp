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
    const std::string name_hash =
        s_ongoing_measurements.empty() ? name : s_ongoing_measurements.top().name_hash + "$" + name;
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
    s_ongoing_measurements.pop();

    measurement ms{ongoing.name};
    ms.start = start;
    ms.end = end;
    ms.elapsed = elapsed;
    ms.parent_index = s_ongoing_measurements.empty() ? SIZE_MAX : s_measurements[s_session_name].size();
    s_measurements[s_session_name][ongoing.name_hash].push_back(ms);
}

const char *instrumentor::current_session()
{
    return s_session_name;
}

void instrumentor::clear_measurements(const char *session)
{
    s_measurements[session].clear();
    s_metrics_cache[session].clear();
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
    std::string name = ms.name;
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