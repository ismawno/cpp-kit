#pragma once

#include "kit/utility/uuid.hpp"
#include "kit/utility/type_constraints.hpp"

namespace kit
{
template <Hashable T = uuid> class identifiable
{
  public:
    using id_type = T;
    identifiable() = default;
    identifiable(const T &id) : id(id)
    {
    }

    T id;
};

template <typename T> bool operator==(const identifiable<T> &lhs, const identifiable<T> &rhs)
{
    return lhs.id == rhs.id;
}

template <typename T> bool operator!=(const identifiable<T> &lhs, const identifiable<T> &rhs)
{
    return lhs.id != rhs.id;
}
} // namespace kit

template <typename T> struct std::hash<kit::identifiable<T>>
{
    std::size_t operator()(const kit::identifiable<T> &id) const
    {
        return std::hash<T>()(id.id);
    }
};