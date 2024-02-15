#pragma once

#include "kit/profiling/measurement.hpp"
#include <unordered_map>
#include <string>

namespace kit::perf
{
using ms_container = std::unordered_map<std::string, std::vector<measurement>>;
class node
{
  public:
    struct elapsed
    {
        time per_call;
        time over_calls;
        float relative_percent;
        float total_percent;
    };

    node(const std::string &name_hash, const ms_container *global_measurements);

    node operator[](const std::string &name) const;
    const measurement &operator[](std::size_t index) const;

    node parent() const;

    auto begin() const
    {
        return m_measurements->begin();
    }
    auto end() const
    {
        return m_measurements->end();
    }

    operator const std::vector<measurement> &() const;

    bool exists() const;
    std::size_t size() const;

  private:
    const ms_container *m_global_measurements;
    const std::vector<measurement> *m_measurements;
    std::string m_name_hash;
};
} // namespace kit::perf