#include "kit/internal/pch.hpp"
#include "kit/interface/nameable.hpp"

namespace kit
{
nameable::nameable(const char *name) : m_name(name)
{
}

const char *nameable::name() const
{
    return m_name;
}
void nameable::name(const char *name)
{
    m_name = name;
}
} // namespace kit