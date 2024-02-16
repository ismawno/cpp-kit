#include "kit/internal/pch.hpp"
#include "kit/profiling/node.hpp"

namespace kit::perf
{
node::node(const std::string &name_hash, const ms_container *measurements, metrics_cache *cache)
    : m_name_hash(name_hash), m_global_measurements(measurements), m_measurements(&measurements->at(name_hash)),
      m_cache(cache)
{
}

node node::operator[](const std::string &name) const
{
    const std::string name_hash = m_name_hash + "$" + name;
    const node n = node{name_hash, m_global_measurements, m_cache};
    KIT_ASSERT_ERROR(n.exists(), "The node with name {0} does not exist", name);
    return n;
}

const measurement &node::operator[](std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_measurements->size(), "Index {0} out of bounds", index);
    return m_measurements->at(index);
}

node node::parent() const
{
    const auto last_dollar = m_name_hash.find_last_of('$');
    KIT_ASSERT_ERROR(last_dollar != std::string::npos, "The node with name hash {0} does not have a parent",
                     m_name_hash);
    return node{m_name_hash.substr(0, last_dollar), m_global_measurements, m_cache};
}
std::unordered_set<std::string> node::children() const
{
    std::unordered_set<std::string> result;
    const std::size_t pcount = std::count(m_name_hash.begin(), m_name_hash.end(), '$');
    for (const auto &[name_hash, _] : *m_global_measurements)
    {
        const std::size_t count = std::count(name_hash.begin(), name_hash.end(), '$');
        if (pcount != count + 1)
            continue;
        const auto last_dollar = name_hash.find_last_of('$');
        result.insert(name_hash.substr(last_dollar + 1));
    }
    return result;
}

bool node::exists() const
{
    return m_measurements != nullptr;
}
bool node::has_parent() const
{
    return m_measurements->front().parent_index != SIZE_MAX;
}

const measurement::metrics &node::compute_metrics(const std::size_t index) const
{
    const non_commutative_tuple<std::string, std::size_t> key{m_name_hash, index};
    const auto it = m_cache->find(key);
    if (it != m_cache->end())
        return it->second;

    measurement::metrics &result = m_cache->emplace(key, measurement::metrics{}).first->second;
    const measurement &ms = m_measurements->at(index);

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

measurement::metrics node::average_metrics() const
{
    measurement::metrics result = compute_metrics(0);
    for (std::size_t i = 1; i < size(); ++i)
    {
        const measurement::metrics &ms = compute_metrics(i);
        result.elapsed += ms.elapsed;
        result.relative_percent += ms.relative_percent;
        result.total_percent += ms.total_percent;
    }
    const std::size_t sz = size();
    result.elapsed /= sz;
    result.relative_percent /= sz;
    result.total_percent /= sz;
    return result;
}

time node::total_elapsed_time() const
{
    time result{};
    for (const measurement &ms : *m_measurements)
        result += ms.elapsed;
    return result;
}
const char *node::name() const
{
    return m_measurements->front().name;
}
const std::string &node::name_hash() const
{
    return m_name_hash;
}

std::size_t node::size() const
{
    return exists() ? m_measurements->size() : 0;
}

node::operator const std::vector<measurement> &() const
{
    return *m_measurements;
}

} // namespace kit::perf