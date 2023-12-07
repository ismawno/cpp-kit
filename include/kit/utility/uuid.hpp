#ifndef KIT_UUID_HPP
#define KIT_UUID_HPP

#include <cstdint>
#include <functional>
#include <string>

namespace kit
{
class uuid
{
  public:
    uuid();
    explicit uuid(std::uint64_t id);

    operator std::uint64_t() const;

    static const std::string &name_from_id(uuid id, std::uint32_t min_characters = 3, std::uint32_t max_characters = 8);

  private:
    std::uint64_t m_uuid;
};

bool operator==(const uuid &id1, const uuid &id2);
bool operator!=(const uuid &id1, const uuid &id2);

} // namespace kit

namespace std
{
template <> struct hash<kit::uuid>
{
    size_t operator()(const kit::uuid &id) const;
};
} // namespace std

#endif