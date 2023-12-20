#pragma once

#include "kit/interface/identifiable.hpp"

#include <functional>

namespace kit
{
template <typename... Ts> class callback : public identifiable<>
{
  public:
    callback(std::function<void(Ts...)> &&cb) : m_callback(cb)
    {
    }
    callback(const std::function<void(Ts...)> &cb = nullptr) : m_callback(cb)
    {
    }

    void operator()(Ts &&...args) const
    {
        KIT_ASSERT_ERROR(m_callback, "The callback must not be null")
        m_callback(std::forward<Ts>(args)...);
    }

  private:
    std::function<void(Ts...)> m_callback;
};
} // namespace kit
