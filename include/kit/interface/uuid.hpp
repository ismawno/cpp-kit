#ifndef KIT_UUID_HPP
#define KIT_UUID_HPP

#include <cstdint>
#include <functional>

namespace kit
{
class uuid
{
  public:
    uuid();
    uuid(std::uint64_t uuid);

    operator std::uint64_t() const;

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