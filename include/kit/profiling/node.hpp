#pragma once

#include "kit/profiling/measurement.hpp"
#include "kit/container/hashable_tuple.hpp"
#include <unordered_map>
#include <string>

namespace kit::perf
{
using ms_container = std::unordered_map<std::string, std::vector<measurement>>;
using metrics_cache = std::unordered_map<non_commutative_tuple<std::string, std::size_t>, measurement::metrics>;
class node
{
  public:
    node(const std::string &name_hash, const ms_container *measurements, metrics_cache *cache);

    node operator[](const std::string &name) const;
    const measurement &operator[](std::size_t index) const;

    const measurement::metrics &compute_metrics(std::size_t index) const;
    measurement::metrics average_metrics() const;

    time total_elapsed_time() const;

    node parent() const;
    std::unordered_set<std::string> children() const;

    auto begin() const
    {
        return m_measurements->begin();
    }
    auto end() const
    {
        return m_measurements->end();
    }

    operator const std::vector<measurement> &() const;

    bool has_parent() const;
    std::size_t size() const;

  private:
    std::string m_name_hash;
    const ms_container *m_global_measurements;
    const std::vector<measurement> *m_measurements;
    metrics_cache *m_cache;

    bool exists() const;
};
} // namespace kit::perf