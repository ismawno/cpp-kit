#pragma once

#include "kit/debug/log.hpp"
#include "kit/utility/type_constraints.hpp"
#include <array>

namespace kit
{
template <typename T, std::size_t Size> class _narray_impl
{
  public:
    template <class... ArrayArgs>
        requires NoCopyCtorOverride<_narray_impl, ArrayArgs...>
    _narray_impl(ArrayArgs &&...args) : m_data({std::forward<ArrayArgs>(args)...})
    {
    }
    const T &operator[](const std::size_t index) const noexcept
    {
        KIT_ASSERT_ERROR(index < m_data.size(), "Index exceeds container size: {0}", index)
        return m_data[index];
    }
    T &operator[](const std::size_t index) noexcept
    {
        KIT_ASSERT_ERROR(index < m_data.size(), "Index exceeds container size: {0}", index)
        return m_data[index];
    }

    const T *data() const noexcept
    {
        return m_data.data();
    }
    T *data() noexcept
    {
        return m_data.data();
    }

    const T &front() const noexcept
    {
        return m_data.front();
    }
    const T &back() const noexcept
    {
        return m_data.back();
    }

    T &front() noexcept
    {
        return m_data.front();
    }
    T &back() noexcept
    {
        return m_data.back();
    }

    auto cbegin() const noexcept
    {
        return m_data.cbegin();
    }
    auto cend() const noexcept
    {
        return m_data.cend();
    }

    auto begin() const noexcept
    {
        return m_data.begin();
    }
    auto end() const noexcept
    {
        return m_data.end();
    }

    auto begin() noexcept
    {
        return m_data.begin();
    }
    auto end() noexcept
    {
        return m_data.end();
    }

    auto rbegin() const noexcept
    {
        return m_data.rbegin();
    }
    auto rend() const noexcept
    {
        return m_data.rend();
    }

    auto rbegin() noexcept
    {
        return m_data.rbegin();
    }
    auto rend() noexcept
    {
        return m_data.rend();
    }

    static constexpr std::size_t size() noexcept
    {
        return Size;
    }

  private:
    std::array<T, Size> m_data;
};

template <typename T, std::size_t Size, std::size_t... Shape>
class narray : public _narray_impl<narray<T, Shape...>, Size>
{
  public:
    template <class... ArrayArgs>
        requires NoCopyCtorOverride<_narray_impl, ArrayArgs...>
    narray(ArrayArgs &&...args) : _narray_impl<narray<T, Shape...>, Size>(std::forward<ArrayArgs>(args)...)
    {
    }
};

template <typename T, std::size_t Size> class narray<T, Size> : public _narray_impl<T, Size>
{
  public:
    template <class... ArrayArgs>
        requires NoCopyCtorOverride<_narray_impl, ArrayArgs...>
    narray(ArrayArgs &&...args) : _narray_impl<T, Size>(std::forward<ArrayArgs>(args)...)
    {
    }
};
} // namespace kit