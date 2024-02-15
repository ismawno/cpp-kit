#include "kit/internal/pch.hpp"
#include "kit/profiling/node.hpp"

namespace kit::perf
{
node::node(const std::string &name_hash, const ms_container *measurements, metrics_cache *cache)
    : m_name_hash(name_hash), m_measurements(measurements), m_cache(cache)
{
}

node node::operator[](const std::string &name) const
{
    const std::string name_hash = m_name_hash + "$" + name;
    const node n = node{name_hash, m_measurements, m_cache};
    KIT_ASSERT_ERROR(n.exists(), "The node with name {0} does not exist", name);
    return n;
}

const measurement &node::operator[](std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_measurements->at(m_name_hash).size(), "Index {0} out of bounds", index);
    return m_measurements->at(m_name_hash)[index];
}

node node::parent() const
{
    const auto last_dollar = m_name_hash.find_last_of('$');
    KIT_ASSERT_ERROR(last_dollar != std::string::npos, "The node with name hash {0} does not have a parent",
                     m_name_hash);
    return node{m_name_hash.substr(0, last_dollar), m_measurements, m_cache};
}

bool node::exists() const
{
    return m_measurements->find(m_name_hash) != m_measurements->end();
}
bool node::has_parent() const
{
    return m_name_hash.find('$') != std::string::npos;
}

const measurement::metrics &node::compute_metrics(const std::size_t index) const
{
    const non_commutative_tuple<std::string, std::size_t> key{m_name_hash, index};
    const auto it = m_cache->find(key);
    if (it != m_cache->end())
        return it->second;

    measurement::metrics &result = m_cache->emplace(key, measurement::metrics{}).first->second;
    const measurement &ms = m_measurements->at(m_name_hash)[index];

    result.elapsed = ms.elapsed;
    if (!has_parent())
        return result;

    const node par = parent();
    const measurement::metrics &par_metrics = par.compute_metrics(ms.parent_index);
    result.relative_percent = result.elapsed.as<kit::perf::time::seconds, float>() /
                              par_metrics.elapsed.as<kit::perf::time::seconds, float>();
    result.total_percent = result.relative_percent * par_metrics.total_percent;
    return result;
}

std::size_t node::size() const
{
    return exists() ? m_measurements->at(m_name_hash).size() : 0;
}

node::operator const std::vector<measurement> &() const
{
    return m_measurements->at(m_name_hash);
}

} // namespace kit::perf