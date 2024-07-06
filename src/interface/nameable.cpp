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

template class nameable<const char *>;
template class nameable<std::string>;
} // namespace kit