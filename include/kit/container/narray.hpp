#pragma once

#include <array>

namespace kit
{
template <typename T, std::size_t Size> class _narray_impl
{
  public:
    template <class... ArrayArgs> _narray_impl(ArrayArgs &&...args) : m_data({std::forward<ArrayArgs>(args)...})
    {
    }
    const T &operator[](const std::size_t index) const noexcept
    {
        return m_data[index];
    }
    T &operator[](const std::size_t index) noexcept
    {
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

template <typename T, std::size_t Size, std::size_t... Dim> class narray : public _narray_impl<narray<T, Dim...>, Size>
{
  public:
    template <class... ArrayArgs>
    narray(ArrayArgs &&...args) : _narray_impl<narray<T, Dim...>, Size>(std::forward<ArrayArgs>(args)...)
    {
    }
};

template <typename T, std::size_t Size> class narray<T, Size> : public _narray_impl<T, Size>
{
  public:
    template <class... ArrayArgs> narray(ArrayArgs &&...args) : _narray_impl<T, Size>(std::forward<ArrayArgs>(args)...)
    {
    }
};
} // namespace kit