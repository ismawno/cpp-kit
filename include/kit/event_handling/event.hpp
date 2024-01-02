#pragma once

#include "kit/event_handling/callback.hpp"
#include <vector>

namespace kit
{
template <class... Args> class event final
{
  public:
    event()
    {
        m_callbacks.reserve(10);
    }

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
    void operator()(Args &&...args) const
    {
        for (std::size_t i = m_callbacks.size() - 1; i < m_callbacks.size(); i--)
            m_callbacks[i](std::forward<Args>(args)...);
    }

    const std::vector<callback<Args...>> &callbacks() const
    {
        return m_callbacks;
    }

  private:
    std::vector<callback<Args...>> m_callbacks;
};
} // namespace kit
