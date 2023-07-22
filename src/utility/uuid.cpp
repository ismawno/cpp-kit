#include "kit/internal/pch.hpp"
#include "kit/utility/uuid.hpp"

namespace kit
{
static std::random_device s_device;
static std::mt19937_64 s_eng(s_device());
static std::uniform_int_distribution<uint64_t> s_dist;

uuid::uuid() : m_uuid(s_dist(s_eng))
{
}

template <> uuid::uuid(const std::uint64_t uuid) : m_uuid(uuid)
{
}

uuid::operator uint64_t() const
{
    return m_uuid;
}

bool operator==(const uuid &id1, const uuid &id2)
{
    return (std::uint64_t)id1 == (std::uint64_t)id2;
}
bool operator!=(const uuid &id1, const uuid &id2)
{
    return (std::uint64_t)id1 != (std::uint64_t)id2;
}
} // namespace kit

namespace std
{
size_t hash<kit::uuid>::operator()(const kit::uuid &key) const
{
    return hash<uint64_t>()((uint64_t)key);
}
} // namespace std