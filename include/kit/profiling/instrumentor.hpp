#pragma once

#include "kit/interface/nameable.hpp"
#include "kit/profiling/time.hpp"
#include "kit/profiling/clock.hpp"
#include "kit/profiling/node.hpp"
#include <stack>
#include <sstream>

namespace kit::perf
{
class instrumentor
{
  public:
    struct scoped_session
    {
        scoped_session(const char *name);
        ~scoped_session();
    };
    struct scoped_measurement
    {
        scoped_measurement(const char *name);
        ~scoped_measurement();
    };

    static void begin_session(const char *name);
    static void end_session();

    static void begin_measurement(const char *name);
    static void end_measurement();

    static const char *current_session();
    static bool has_measurements(const char *session);
    static node head_node(const char *session);

    static std::stringstream strstream(const char *session);
    static std::string str(const char *session);

  private:
    struct ongoing_measurement
    {
        const char *name;
        std::string name_hash;
        clock clk;
    };
    instrumentor() = delete;

    static inline std::stack<ongoing_measurement> s_ongoing_measurements{};
    static inline ms_container s_current_measurements{};

    static inline std::unordered_map<const char *, const char *> s_head_node_names{};
    static inline std::unordered_map<const char *, ms_container> s_measurements{};
    static inline std::unordered_map<const char *, metrics_cache> s_metrics_cache{};

    static inline const char *s_session_name = nullptr;
};
} // namespace kit::perf
