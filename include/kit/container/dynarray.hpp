#pragma once

#include "kit/debug/log.hpp"
#include "kit/utility/type_constraints.hpp"
#include <initializer_list>
#include <array>
#include <algorithm>

namespace kit
{
template <typename T, std::size_t Capacity> class dynarray
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = pointer;
    using const_iterator = const_pointer;

    dynarray(const size_type size = 0) : m_size(size)
    {
    }
    dynarray(const size_type size, const_reference value) : m_size(size)
    {
        std::fill(begin(), end(), value);
    }

    template <Iterator<value_type> It> dynarray(It begin, It end) : m_size(std::distance(begin, end))
    {
        KIT_ASSERT_ERROR(m_size <= Capacity, "Data size must not exceed capacity");
        std::copy(begin, end, m_data.begin());
    }

    template <size_type OtherCapacity>
    dynarray(const dynarray<value_type, OtherCapacity> &data)
        requires(OtherCapacity <= Capacity)
        : m_size(data.size())
    {
        std::copy(data.begin(), data.end(), m_data.begin());
    }

    dynarray(std::initializer_list<value_type> data) : m_size(data.size())
    {
        KIT_ASSERT_ERROR(data.size() <= Capacity, "Data size must not exceed capacity");
        std::copy(data.begin(), data.end(), m_data.begin());
    }

    template <size_type OtherCapacity>
    dynarray &operator=(const dynarray<value_type, OtherCapacity> &data)
        requires(OtherCapacity <= Capacity)
    {
        m_size = data.size();
        std::copy(data.begin(), data.end(), m_data.begin());
        return *this;
    }
    operator const std::array<value_type, Capacity> &() const
    {
        return m_data;
    }

    void push_back(const_reference elem)
    {
        KIT_ASSERT_ERROR(m_size < Capacity, "Data size must not exceed capacity");
        m_data[m_size++] = elem;
    }

    void pop_back()
    {
        KIT_ASSERT_ERROR(m_size > 0, "Data size must be greater than zero");
        --m_size;
    }

    void clear()
    {
        m_size = 0;
    }
    bool empty() const
    {
        return m_size == 0;
    }
    void resize(const size_type size)
    {
        KIT_ASSERT_ERROR(size <= Capacity, "Data size must not exceed capacity");
        m_size = size;
    }
    size_type size() const
    {
        return m_size;
    }

    constexpr size_type capacity() const
    {
        return Capacity;
    }

    reference front()
    {
        KIT_ASSERT_ERROR(m_size > 0, "Data size must be greater than zero");
        return m_data[0];
    }
    reference back()
    {
        KIT_ASSERT_ERROR(m_size > 0, "Data size must be greater than zero");
        return m_data[m_size - 1];
    }

    const_reference front() const
    {
        KIT_ASSERT_ERROR(m_size > 0, "Data size must be greater than zero");
        return m_data[0];
    }
    const_reference back() const
    {
        KIT_ASSERT_ERROR(m_size > 0, "Data size must be greater than zero");
        return m_data[m_size - 1];
    }

    // begin and end methods
    iterator begin()
    {
        return m_data.begin();
    }
    iterator end()
    {
        return m_data.begin() + m_size;
    }
    const_iterator begin() const
    {
        return m_data.begin();
    }
    const_iterator end() const
    {
        return m_data.begin() + m_size;
    }
    const_iterator cbegin() const
    {
        return m_data.cbegin();
    }
    const_iterator cend() const
    {
        return m_data.cbegin() + m_size;
    }

    iterator erase(const_iterator pos)
    {
        KIT_ASSERT_ERROR(pos >= cbegin() && pos < cend(), "Iterator must be within range");
        const difference_type offset = std::distance(cbegin(), pos);
        std::copy(pos + 1, cend(), begin() + offset);
        --m_size;

        return begin() + offset;
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        KIT_ASSERT_ERROR(first >= begin() && first <= end(), "Iterator must be within range");
        KIT_ASSERT_ERROR(last >= begin() && last <= end(), "Iterator must be within range");
        KIT_ASSERT_ERROR(first <= last, "First iterator must be less than or equal to last iterator");
        const difference_type offset = std::distance(begin(), first);
        if (last != end())
            std::copy(last, end(), first);
        m_size -= std::distance(first, last);
        return begin() + offset;
    }

    reference operator[](const size_type index)
    {
        KIT_ASSERT_ERROR(index < m_size, "Index must be less than size");
        return m_data[index];
    }
    const_reference operator[](const size_type index) const
    {
        KIT_ASSERT_ERROR(index < m_size, "Index must be less than size");
        return m_data[index];
    }

  private:
    std::array<value_type, Capacity> m_data;
    size_type m_size;
};
} // namespace kit