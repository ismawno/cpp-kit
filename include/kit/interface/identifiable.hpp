#ifndef KIT_IDENTIFIABLE_HPP
#define KIT_IDENTIFIABLE_HPP

#include "kit/utility/uuid.hpp"

namespace kit
{
template <typename T = uuid> class identifiable
{
  public:
    identifiable() = default;
    identifiable(const T &id) : m_id(id)
    {
    }

    const T &id() const
    {
        return m_id;
    }
    void id(const T &id)
    {
        m_id = id;
    }

  private:
    T m_id;
};

template <typename T> bool operator==(const identifiable<T> &lhs, const identifiable<T> &rhs)
{
    return lhs.id() == rhs.id();
}

template <typename T> bool operator!=(const identifiable<T> &lhs, const identifiable<T> &rhs)
{
    return lhs.id() != rhs.id();
}
} // namespace kit

namespace std
{
template <typename T> struct hash<kit::identifiable<T>>
{
    size_t operator()(const kit::identifiable<T> &id) const
    {
        return hash<T>()(id.id());
    }
};
} // namespace std

#endif