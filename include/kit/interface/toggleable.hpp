#pragma once

#include "kit/utility/type_constraints.hpp"

namespace kit
{
class toggleable
{
  public:
    toggleable(bool enabled = true);
    virtual ~toggleable() = default;

    bool enabled() const;
    virtual void enabled(bool enabled);

    void enable();
    void disable();

  protected:
    bool m_enabled;
};

template <typename T>
concept Toggleable = kit::DerivedFrom<T, toggleable>;
;
} // namespace kit
