#ifndef KIT_EVENT_HPP
#define KIT_EVENT_HPP

#include "kit/event_handling/callback.hpp"
#include <vector>

namespace kit
{
template <class... Ts> class event final
{
  public:
    event()
    {
        m_callbacks.reserve(10);
    }

    event &operator+=(const callback<Ts...> &cb)
    {
        m_callbacks.push_back(cb);
        return *this;
    }
    event &operator-=(const callback<Ts...> &cb)
    {
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
            if (*it == cb)
            {
                m_callbacks.erase(it);
                return *this;
            }
        return *this;
    }
    void operator()(Ts &&...args) const
    {
        for (std::size_t i = m_callbacks.size() - 1; i < m_callbacks.size(); i--)
            m_callbacks[i](std::forward<Ts>(args)...);
    }

    const std::vector<callback<Ts...>> &callbacks() const
    {
        return m_callbacks;
    }

  private:
    std::vector<callback<Ts...>> m_callbacks;
};
} // namespace kit

#endif