#include "kit/internal/pch.hpp"
#include "kit/interface/identifiable.hpp"

namespace kit
{
identifiable::identifiable(const uuid id) : m_uuid(id)
{
}

uuid identifiable::id() const
{
    return m_uuid;
}
void identifiable::id(const uuid id)
{
    m_uuid = id;
}

bool operator==(const identifiable &lhs, const identifiable &rhs)
{
    return lhs.id() == rhs.id();
}
bool operator!=(const identifiable &lhs, const identifiable &rhs)
{
    return lhs.id() != rhs.id();
}
} // namespace kit