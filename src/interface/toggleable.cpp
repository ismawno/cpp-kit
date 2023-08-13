#include "kit/internal/pch.hpp"
#include "kit/interface/toggleable.hpp"

namespace kit
{
toggleable::toggleable(const bool enabled) : enabled(enabled)
{
}

void toggleable::enable()
{
    enabled = true;
}
void toggleable::disable()
{
    enabled = false;
}
} // namespace kit