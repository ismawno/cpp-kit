#ifndef KIT_IDENTIFIABLE_HPP
#define KIT_IDENTIFIABLE_HPP

#include "kit/utility/uuid.hpp"

namespace kit
{
class identifiable
{
  public:
    identifiable() = default;
    identifiable(uuid id);

    uuid id() const;
    void id(uuid id);

  private:
    uuid m_uuid;
};

bool operator==(const identifiable &lhs, const identifiable &rhs);
bool operator!=(const identifiable &lhs, const identifiable &rhs);
} // namespace kit

namespace std
{
template <> struct hash<kit::identifiable>
{
    size_t operator()(const kit::identifiable &id) const;
};
} // namespace std

#endif