#include "kit/internal/pch.hpp"
#include "kit/interface/nameable.hpp"

namespace kit
{
template <ValidNameType T> nameable<T>::nameable(const T &name) : m_name(name)
{
}

template <ValidNameType T> const T &nameable<T>::name() const
{
    return m_name;
}
} // namespace kit