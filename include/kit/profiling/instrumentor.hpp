#pragma once

#include "kit/profiling/measurement.hpp"
#include "kit/profiling/clock.hpp"
#include <stack>
#include <sstream>

namespace kit::perf
{
class instrumentor
{
  public:
    struct scoped_measurement
    {
        scoped_measurement(const char *name);
        ~scoped_measurement();
    };

    static instrumentor &main();

    void begin_measurement(const char *name);
    void end_measurement();

    const measurement &operator[](const char *name) const;
    const measurement &operator[](std::size_t index) const;
    const std::vector<measurement> &measurements() const;

    std::size_t size() const;
    bool empty() const;

    auto begin() const
    {
        return m_registry.flat.begin();
    }
    auto end() const
    {
        return m_registry.flat.end();
    }

  private:
    struct ongoing_measurement
    {
        const char *name;
        clock clk;
    };
    struct measurement_registry
    {
        std::vector<measurement> flat;
        std::unordered_map<const char *, std::size_t> map;
    };

    std::stack<ongoing_measurement> m_ongoing_measurements{};

    measurement_registry m_ongoing_registry;
    measurement_registry m_registry;
};
} // namespace kit::perf
