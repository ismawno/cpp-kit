#ifndef KIT_TRACK_PTR_HPP
#define KIT_TRACK_PTR_HPP

#include "kit/utility/uuid.hpp"
#include "kit/utility/callback.hpp"
#include "kit/container/track_vector.hpp"
#include "kit/debug/log.hpp"
#include <cstddef>

namespace kit
{
template <typename T> class const_track_ptr
{
  public:
    const_track_ptr() = default;
    const_track_ptr(const track_vector<T> *vector, const std::size_t index = 0) : m_vector(vector), m_index(index)
    {
        KIT_ASSERT_ERROR(m_vector != nullptr && m_index < m_vector->size(),
                         "A track ptr cannot have an index greater or equal to the vector size!")
        if (vector)
        {
            m_callback = erase_callback();
            vector->on_erase += m_callback;
        }
    }
    const_track_ptr(const const_track_ptr &other) : const_track_ptr(other.m_vector, other.m_index)
    {
    }

    ~const_track_ptr()
    {
        if (m_vector)
            m_vector->on_erase -= m_callback;
    }

    const_track_ptr &operator=(const const_track_ptr &other)
    {
        if (m_vector)
            m_vector->on_erase -= m_callback;
        m_vector = other.m_vector;
        m_index = other.m_index;
        if (m_vector)
        {
            m_callback = erase_callback();
            m_vector->on_erase += m_callback;
        }
        return *this;
    }

    operator bool() const
    {
        KIT_ASSERT_ERROR(!m_vector || m_index < m_vector->size(),
                         "A track ptr cannot have an index greater or equal to the vector size!")
        return m_vector != nullptr;
    }

    const T *raw() const
    {
        return m_vector ? &((*m_vector)[m_index]) : nullptr;
    }
    const T *operator->() const
    {
        KIT_ASSERT_ERROR(*this, "Cannot dereference a null pointer")
        return &((*m_vector)[m_index]);
    }
    const T &operator*() const
    {
        KIT_ASSERT_ERROR(*this, "Cannot dereference a null pointer")
        return (*m_vector)[m_index];
    }

    bool operator==(const const_track_ptr &other)
    {
        return m_vector == other.m_vector && m_index == other.m_index;
    }
    bool operator!=(const const_track_ptr &other)
    {
        return !(*this == other);
    }

    callback<std::size_t> erase_callback()
    {
        return callback<std::size_t>([this](const std::size_t removed) {
            KIT_ASSERT_CRITICAL(m_vector, "Underlying track vector cannot have a callback from a null track pointer")
            if (removed == m_index)
            {
                m_vector->on_erase -= m_callback;
                m_vector = nullptr;
                return;
            }
            if (removed < m_index)
                m_index--;
        });
    }

  private:
    const track_vector<T> *m_vector = nullptr;
    std::size_t m_index;
    callback<std::size_t> m_callback{nullptr};
};

template <typename T> class track_ptr
{
  public:
    track_ptr() = default;
    track_ptr(track_vector<T> *vector, const std::size_t index = 0) : m_vector(vector), m_index(index)
    {
        KIT_ASSERT_ERROR(m_vector != nullptr && m_index < m_vector->size(),
                         "A track ptr cannot have an index greater or equal to the vector size!")
        if (vector)
        {
            m_callback = erase_callback();
            vector->on_erase += m_callback;
        }
    }
    track_ptr(const track_ptr &other) : track_ptr(other.m_vector, other.m_index)
    {
    }

    ~track_ptr()
    {
        if (m_vector)
            m_vector->on_erase -= m_callback;
    }

    track_ptr &operator=(const track_ptr &other)
    {
        if (m_vector)
            m_vector->on_erase -= m_callback;
        m_vector = other.m_vector;
        m_index = other.m_index;
        if (m_vector)
        {
            m_callback = erase_callback();
            m_vector->on_erase += m_callback;
        }
        return *this;
    }

    operator bool() const
    {
        KIT_ASSERT_ERROR(!m_vector || m_index < m_vector->size(),
                         "A track ptr cannot have an index greater or equal to the vector size!")
        return m_vector != nullptr;
    }
    operator const_track_ptr<T>() const
    {
        return const_track_ptr(m_vector, m_index);
    }

    T *raw() const
    {
        return m_vector ? &((*m_vector)[m_index]) : nullptr;
    }
    T *operator->() const
    {
        KIT_ASSERT_ERROR(*this, "Cannot dereference a null pointer")
        return &((*m_vector)[m_index]);
    }
    T &operator*() const
    {
        KIT_ASSERT_ERROR(*this, "Cannot dereference a null pointer")
        return (*m_vector)[m_index];
    }

    bool operator==(const track_ptr &other)
    {
        return m_vector == other.m_vector && m_index == other.m_index;
    }
    bool operator!=(const track_ptr &other)
    {
        return !(*this == other);
    }

    callback<std::size_t> erase_callback()
    {
        return callback<std::size_t>([this](const std::size_t removed) {
            if (removed == m_index)
            {
                m_vector->on_erase -= m_callback;
                m_vector = nullptr;
                return;
            }
            if (removed < m_index)
                m_index--;
        });
    }

  private:
    track_vector<T> *m_vector = nullptr;
    std::size_t m_index = 0;
    callback<std::size_t> m_callback{nullptr};
};
} // namespace kit

#endif