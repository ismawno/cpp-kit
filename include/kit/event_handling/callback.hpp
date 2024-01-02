#pragma once

#include "kit/interface/identifiable.hpp"

#include <functional>

namespace kit
{
template <typename... Args> class callback : public identifiable<>
{
  public:
    using task = std::function<void(Args...)>;
    callback(const task &tk = nullptr) : m_task(tk)
    {
    }

    void operator()(Args &&...args) const
    {
        KIT_ASSERT_ERROR(m_task, "The task must not be null")
        m_task(std::forward<Args>(args)...);
    }

    operator bool()
    {
        return m_task != nullptr;
    }

  private:
    task m_task;
};
} // namespace kit
