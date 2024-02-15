#include "kit/internal/pch.hpp"
#include "kit/profiling/node.hpp"

namespace kit::perf
{
node::node(const std::string &name_hash, const ms_container *global_measurements)
    : m_global_measurements(global_measurements), m_name_hash(name_hash)
{
    const auto &measurements = m_global_measurements->find(name_hash);
    m_measurements = measurements != m_global_measurements->end() ? &measurements->second : nullptr;
}

node node::operator[](const std::string &name) const
{
    const std::string name_hash = m_name_hash + "$" + name;
    return node{name_hash, m_global_measurements};
}

const measurement &node::operator[](std::size_t index) const
{
    KIT_ASSERT_ERROR(exists(), "The node with name hash {0} does not exist", m_name_hash);
    return (*m_measurements)[index];
}

node node::parent() const
{
    const auto last_dollar = m_name_hash.find_last_of('$');
    KIT_ASSERT_ERROR(last_dollar != std::string::npos, "The node with name hash {0} does not have a parent",
                     m_name_hash);
    return node{m_name_hash.substr(0, last_dollar), m_global_measurements};
}

bool node::exists() const
{
    return m_measurements != nullptr;
}

std::size_t node::size() const
{
    return exists() ? m_measurements->size() : 0;
}

node::operator const std::vector<measurement> &() const
{
    KIT_ASSERT_ERROR(exists(), "The node with name hash {0} does not exist", m_name_hash);
    return *m_measurements;
}

} // namespace kit::perf