#ifndef KIT_TRACK_PTR_HPP
#define KIT_TRACK_PTR_HPP

#include "kit/utility/uuid.hpp"
#include "kit/interface/identifiable.hpp"
#include "kit/debug/log.hpp"
#include <vector>
#include <cstddef>

namespace kit
{
template <typename T, typename ID = uuid> class const_track_ptr : public identifiable<ID>
{
    static_assert(std::is_base_of<identifiable<ID>, T>::value,
                  "The type of a track_ptr must be identifiable with the ID type");

  public:
    const_track_ptr() = default;
    const_track_ptr(const std::vector<T> *vector, const std::size_t index = 0)
        : identifiable<ID>(vector ? (*vector)[index].id : ID()), m_vector(vector), m_index(index)
    {
        KIT_ASSERT_ERROR(!m_vector || m_index < m_vector->size(),
                         "A track ptr cannot have an index greater or equal to the vector size!")
    }

    operator bool() const
    {
        if (!m_vector || m_index == SIZE_T_MAX)
            return false;
        if (m_index < m_vector->size() && (*m_vector)[m_index].id == this->id)
            return true;

        for (std::size_t i = 0; i < m_vector->size(); i++)
            if ((*m_vector)[i].id == this->id)
            {
                m_index = i;
                return true;
            }
        m_index = SIZE_T_MAX;
        return false;
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

  private:
    const std::vector<T> *m_vector = nullptr;
    mutable std::size_t m_index;
};

template <typename T, typename ID = uuid> class track_ptr : public identifiable<ID>
{
    static_assert(std::is_base_of<identifiable<ID>, T>::value,
                  "The type of a track_ptr must be identifiable with the ID type");

  public:
    track_ptr() = default;
    track_ptr(std::vector<T> *vector, const std::size_t index = 0)
        : identifiable<ID>(vector ? (*vector)[index].id : ID()), m_vector(vector), m_index(index)
    {
        KIT_ASSERT_ERROR(!m_vector || m_index < m_vector->size(),
                         "A track ptr cannot have an index greater or equal to the vector size!")
    }

    operator bool() const
    {
        if (!m_vector || m_index == SIZE_T_MAX)
            return false;
        if (m_index < m_vector->size() && (*m_vector)[m_index].id == this->id)
            return true;

        for (std::size_t i = 0; i < m_vector->size(); i++)
            if ((*m_vector)[i].id == this->id)
            {
                m_index = i;
                return true;
            }
        m_index = SIZE_T_MAX;
        return false;
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

    operator const_track_ptr<T, ID>() const
    {
        const_track_ptr<T, ID> ptr{m_vector, m_index};
        ptr.id = this->id;
        return ptr;
    }

  private:
    std::vector<T> *m_vector = nullptr;
    mutable std::size_t m_index;
};
} // namespace kit

#endif