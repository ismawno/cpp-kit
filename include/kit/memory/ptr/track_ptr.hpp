#pragma once

#include "kit/utility/uuid.hpp"
#include "kit/interface/identifiable.hpp"
#include "kit/debug/log.hpp"
#include <cstddef>

namespace kit
{
template <typename Container>
concept IDContainer = requires(Container a) {
    typename Container::value_type;
    requires Identifiable<typename Container::value_type>;
    {
        a.size()
    } -> std::convertible_to<std::size_t>;
    {
        a[0]
    } -> std::convertible_to<const typename Container::value_type &>;
};

template <typename From, typename To>
concept ConstConvertibleContainer =
    IDContainer<From> && IDContainer<To> && std::is_same_v<std::remove_const_t<From>, std::remove_const_t<To>> &&
    !(std::is_const_v<From> && !std::is_const_v<To>);

template <IDContainer Container> class track_ptr : public identifiable<typename Container::value_type::id_type>
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
        requires ConstConvertibleContainer<OtherContainer, Container>
    track_ptr(const track_ptr<OtherContainer> &other) : m_container(other.m_container), m_index(other.m_index)
    {
        this->id = other.id;
    }

    template <typename OtherContainer> track_ptr &operator=(const track_ptr<OtherContainer> &other)
    {
        m_container = other.m_container;
        m_index = other.m_index;
        this->id = other.id;
        return *this;
    }

    validity validate() const
    {
        if (!m_container || m_index == SIZE_MAX)
            return validity::NOT_VALID;
        if (m_index < m_container->size() && (*m_container)[m_index].id == this->id)
            return validity::VALID;
        if (m_index > 0 && (*m_container)[m_index - 1].id == this->id) // More likely to be shifted
        {
            m_index--;
            return validity::VALID_MUTATED;
        }

        for (std::size_t i = 0; i < m_container->size(); i++)
            if ((*m_container)[i].id == this->id)
            {
                m_index = i;
                return validity::VALID_MUTATED;
            }
        m_index = SIZE_MAX;
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

    template <typename OtherContainer>
    operator track_ptr<OtherContainer>()
        requires ConstConvertibleContainer<Container, OtherContainer>
    {
        track_ptr<OtherContainer> ptr{m_container, m_index};
        ptr.id = this->id;
        return ptr;
    }

  private:
    Container *m_container = nullptr;
    mutable std::size_t m_index;

    template <IDContainer OtherContainer> friend class track_ptr;
};
} // namespace kit

template <kit::IDContainer Container> struct std::hash<kit::track_ptr<Container>>
{
    std::size_t operator()(const kit::track_ptr<Container> &ptr) const
    {
        return std::hash<kit::uuid>()(ptr.id);
    }
};
