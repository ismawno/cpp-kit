#pragma once

#include "kit/interface/identifiable.hpp"

#include <functional>

namespace kit
{
template <typename... Args> class callback : public identifiable<>
{
  public:
    using fun = std::function<void(Args...)>;
    callback(const fun &fn = nullptr) : m_fun(fn)
    {
    }

    void operator()(Args &&...args) const
    {
        KIT_ASSERT_ERROR(m_fun, "The callback must not be null")
        m_fun(std::forward<Args>(args)...);
    }

    operator bool()
    {
        return m_fun != nullptr;
    }

  private:
    fun m_fun;
};
} // namespace kit
