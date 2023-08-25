#ifndef KIT_TRACK_PTR_HPP
#define KIT_TRACK_PTR_HPP

#include "kit/utility/uuid.hpp"
#include "kit/interface/identifiable.hpp"
#include "kit/debug/log.hpp"
#include <vector>
#include <cstddef>

namespace kit
{
template <typename Container> class track_ptr : public identifiable<typename Container::value_type::id_type>
{
    using T = typename Container::value_type;
    using ID = typename T::id_type;

  public:
    enum class validity
    {
        VALID,
        NOT_VALID,
        VALID_MUTATED
    };

    track_ptr() = default;
    track_ptr(Container *container, const std::size_t index = 0)
        : identifiable<ID>(container ? (*container)[index].id : ID()), m_container(container), m_index(index)
    {
        KIT_ASSERT_ERROR(!m_container || m_index < m_container->size(),
                         "A track ptr cannot have an index greater or equal to the container size!");
    }

    template <typename OtherContainer>
    track_ptr(const track_ptr<OtherContainer> &other) : m_container(other.m_container), m_index(other.m_index)
    {
        this->id = other.id;
    }

    template <typename OtherContainer> track_ptr &operator=(const track_ptr<OtherContainer> &other)
    {
        m_container = other.m_container;
        m_index = other.m_index;
        this->id = other.id;
    }

    validity validate() const
    {
        if (!m_container || m_index == SIZE_T_MAX)
            return validity::NOT_VALID;
        if (m_index < m_container->size() && (*m_container)[m_index].id == this->id)
            return validity::VALID;

        for (std::size_t i = 0; i < m_container->size(); i++)
            if ((*m_container)[i].id == this->id)
            {
                m_index = i;
                return validity::VALID_MUTATED;
            }
        m_index = SIZE_T_MAX;
        return validity::NOT_VALID;
    }

    operator bool() const
    {
        return validate() != validity::NOT_VALID;
    }

    auto *raw() const
    {
        return m_container ? &((*m_container)[m_index]) : nullptr;
    }
    auto *operator->() const
    {
        KIT_ASSERT_ERROR(validate() == validity::VALID, "Cannot dereference a null or shifted pointer")
        return &((*m_container)[m_index]);
    }
    auto &operator*() const
    {
        KIT_ASSERT_ERROR(validate() == validity::VALID, "Cannot dereference a null or shifted pointer")
        return (*m_container)[m_index];
    }

    template <typename ConstContainer> operator track_ptr<ConstContainer>()
    {
        static_assert(std::is_same_v<typename std::remove_const<Container>::type,
                                     typename std::remove_const<ConstContainer>::type>,
                      "Container must be the same type for a cast to be possible");
        static_assert(std::is_const_v<ConstContainer>,
                      "Output track ptr container must be const for a cast to be possible");
        static_assert(!std::is_const_v<Container>,
                      "Input track ptr container must not be const for a cast to be possible");
        track_ptr<ConstContainer> ptr{m_container, m_index};
        ptr.id = this->id;
        return ptr;
    }

  private:
    Container *m_container = nullptr;
    mutable std::size_t m_index;

    template <typename OtherContainer> friend class track_ptr;
};

template <typename T, class... Args> using vector_ptr = track_ptr<std::vector<T, Args...>>;
template <typename T, class... Args> using const_vector_ptr = track_ptr<const std::vector<T, Args...>>;
} // namespace kit

#endif