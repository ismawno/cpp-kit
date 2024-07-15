#include "kit/internal/pch.hpp"
#include "kit/profiling/instrumentor.hpp"
#include "kit/utility/utils.hpp"
#include <mutex>

namespace kit::perf
{
static std::mutex mutex;

void instrumentor::begin_measurement(const char *name)
{
    KIT_ASSERT_ERROR(name, "Measurement name must not be null")
    std::scoped_lock lock(mutex);
    m_ongoing_measurements.push(ongoing_measurement{name, clock{}});
}

void instrumentor::end_measurement()
{
    KIT_ASSERT_ERROR(!m_ongoing_measurements.empty(), "Cannot end a measurement without beginning one")

    const ongoing_measurement &ongoing = m_ongoing_measurements.top();
    const time elapsed = ongoing.clk.elapsed();
    const char *name = ongoing.name;

    const auto it = m_ongoing_registry.map.find(name);
    std::scoped_lock lock(mutex);
    if (it != m_ongoing_registry.map.end())
    {
        measurement &ms = m_ongoing_registry.flat[it->second];
        ms.average = (ms.average * ms.calls + elapsed) / (ms.calls + 1);
        ms.cumulative += elapsed;
        ms.calls++;
    }
    else
    {
        m_ongoing_registry.map.emplace(name, m_ongoing_registry.flat.size());
        measurement &ms = m_ongoing_registry.flat.emplace_back();
        ms.name = name;
        ms.average = elapsed;
        ms.cumulative = elapsed;
        ms.calls = 1;
    }
    m_ongoing_measurements.pop();
    if (m_ongoing_measurements.empty()) [[unlikely]]
    {
        std::swap(m_ongoing_registry, m_registry);
        m_ongoing_registry.flat.clear();
        m_ongoing_registry.map.clear();
    }
}

const measurement &instrumentor::operator[](const char *name) const
{
    KIT_ASSERT_ERROR(m_registry.map.contains(name), "Measurement not found")
    return m_registry.flat[m_registry.map.at(name)];
}

const measurement &instrumentor::operator[](std::size_t index) const
{
    KIT_ASSERT_ERROR(index < m_registry.flat.size(), "Index out of bounds")
    return m_registry.flat[index];
}

const std::vector<measurement> &instrumentor::measurements() const
{
    return m_registry.flat;
}

bool instrumentor::contains(const char *name) const
{
    return m_registry.map.contains(name);
}

std::size_t instrumentor::size() const
{
    return m_registry.flat.size();
}

bool instrumentor::empty() const
{
    return m_registry.flat.empty();
}
instrumentor &instrumentor::main()
{
    static instrumentor instance;
    return instance;
}

instrumentor::scoped_measurement::scoped_measurement(const char *name)
{
    instrumentor::main().begin_measurement(name);
}
instrumentor::scoped_measurement::~scoped_measurement()
{
    instrumentor::main().end_measurement();
}
} // namespace kit::perf