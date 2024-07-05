#include "kit/internal/pch.hpp"
#include "kit/interface/toggleable.hpp"

namespace kit
{
toggleable::toggleable(const bool enabled) : m_enabled(enabled)
{
}

bool toggleable::enabled() const
{
    return m_enabled;
}
void toggleable::enabled(const bool enabled)
{
    m_enabled = enabled;
}

void toggleable::enable()
{
    enabled(true);
}
void toggleable::disable()
{
    enabled(false);
}
} // namespace kit