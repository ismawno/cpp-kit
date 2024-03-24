#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace kit
{
class uuid
{
  public:
    uuid() = default;
    explicit uuid(std::uint64_t id);

    operator std::uint64_t() const;

    static uuid random();
    static const std::string &name_from_id(uuid id, std::uint32_t min_characters = 3, std::uint32_t max_characters = 8);
    static const std::string &name_from_ptr(const void *ptr, std::uint32_t min_characters = 3,
                                            std::uint32_t max_characters = 8);

  private:
    std::uint64_t m_uuid;
};

bool operator==(const uuid &id1, const uuid &id2);
bool operator!=(const uuid &id1, const uuid &id2);

} // namespace kit

template <> struct std::hash<kit::uuid>
{
    std::size_t operator()(const kit::uuid &id) const;
};
