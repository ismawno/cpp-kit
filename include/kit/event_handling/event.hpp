#pragma once

#include "kit/debug/log.hpp"
#include "kit/event_handling/callback.hpp"
#include <vector>

namespace kit
{
template <class... Args> class event
{
  public:
    event &operator+=(const callback<Args...> &cb)
    {
        m_callbacks.push_back(cb);
        return *this;
    }
    event &operator-=(const callback<Args...> &cb)
    {
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
            if (*it == cb)
            {
                m_callbacks.erase(it);
                return *this;
            }
        KIT_WARN("Callback was not found!")
        return *this;
    }
    void operator()(Args... args) const
    {
        for (std::size_t i = m_callbacks.size() - 1; i < m_callbacks.size(); i--)
            m_callbacks[i](args...);
    }

    const callback<Args...> &operator[](const std::size_t index)
    {
        KIT_ASSERT_ERROR(index < m_callbacks.size(), "Index exceeds container size: {0}", index)
        return m_callbacks[index];
    }
    std::size_t size() const
    {
        return m_callbacks.size();
    }
    bool empty() const
    {
        return m_callbacks.empty();
    }

    auto begin() const
    {
        return m_callbacks.begin();
    }
    auto end() const
    {
        return m_callbacks.end();
    }

    auto begin()
    {
        return m_callbacks.begin();
    }
    auto end()
    {
        return m_callbacks.end();
    }

  private:
    std::vector<callback<Args...>> m_callbacks;
};
} // namespace kit
