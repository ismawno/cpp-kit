#ifndef KIT_TRACK_VECTOR_HPP
#define KIT_TRACK_VECTOR_HPP

#include "kit/utility/event.hpp"
#include "kit/interface/indexable.hpp"

#include <vector>

namespace kit
{
template <typename T, typename Alloc = std::allocator<T>> class track_vector
{
  public:
    template <typename... VectorArgs> track_vector(VectorArgs &&...args) : m_vector(std::forward<VectorArgs>(args)...)
    {
    }
    track_vector(std::initializer_list<T> lst) : m_vector(lst)
    {
    }
    ~track_vector()
    {
        clear();
    }
    mutable event<std::size_t> on_erase;

    T &operator[](const std::size_t index)
    {
        return m_vector[index];
    }
    const T &operator[](const std::size_t index) const
    {
        return m_vector[index];
    }

    template <class... EmplaceArgs> T &emplace_back(EmplaceArgs &&...args)
    {
        T &elm = m_vector.emplace_back(std::forward<EmplaceArgs>(args)...);

        if constexpr (std::is_base_of<indexable, T>::value)
            elm.index(m_vector.size() - 1);
        return elm;
    }
    template <class... PushArgs> void push_back(PushArgs &&...args)
    {
        m_vector.push_back(std::forward<PushArgs>(args)...);
        if constexpr (std::is_base_of<indexable, T>::value)
            m_vector.back().index(m_vector.size() - 1);
    }

    void erase(std::size_t index)
    {
        m_vector.erase(m_vector.begin() + (long)index);
        KIT_DEBUG("Track vector erase: {0} callbacks prior", on_erase.callbacks().size())
        on_erase(std::move(index));
        if constexpr (std::is_base_of<indexable, T>::value)
            for (std::size_t i = index; i < m_vector.size(); i++)
                m_vector[i].index(i);
        KIT_DEBUG("Track vector erase: {0} callbacks later", on_erase.callbacks().size())
    }
    void erase(const T &elm)
    {
        static_assert(std::is_base_of<indexable, T>::value, "Can only erase with vector element if type is indexable");
        erase(elm.index());
    }

    auto erase(typename std::vector<T>::const_iterator it)
    {
        static_assert(std::is_base_of<indexable, T>::value,
                      "Can only erase with vector iterators if type is indexable");

        KIT_DEBUG("Track vector erase: {0} callbacks prior", on_erase.callbacks().size())
        on_erase(it->index());
        KIT_DEBUG("Track vector erase: {0} callbacks later", on_erase.callbacks().size())
        for (std::size_t i = it->index() + 1; i < m_vector.size(); i++)
            m_vector[i].index(i - 1);

        return m_vector.erase(it);
    }
    auto erase(typename std::vector<T>::const_iterator from, typename std::vector<T>::const_iterator to)
    {
        static_assert(std::is_base_of<indexable, T>::value,
                      "Can only erase with vector iterators if type is indexable");
        KIT_ASSERT_ERROR(from < to, "'from' iterator must be lower than 'to'")

        KIT_DEBUG("Track vector erase: {0} callbacks prior", on_erase.callbacks().size())
        for (auto it = to - 1; it >= from; --it)
            on_erase(it->index());
        KIT_DEBUG("Track vector erase: {0} callbacks later", on_erase.callbacks().size())

        const std::size_t diff = to->index() - from->index();
        for (std::size_t i = to->index() + 1; i < m_vector.size(); i++)
            m_vector[i].index(i - diff);

        return m_vector.erase(from, to);
    }

    void clear()
    {
        KIT_DEBUG("Track vector clear: {0} callbacks prior", on_erase.callbacks().size())
        for (std::size_t i = m_vector.size() - 1; i < m_vector.size(); i--)
            on_erase(std::move(i));
        KIT_DEBUG("Track vector clear: {0} callbacks later", on_erase.callbacks().size())

        KIT_ASSERT_ERROR(on_erase.callbacks().empty(),
                         "When clearing a track_vector, all pointers should be invalidated (i.e no more on_erase "
                         "callbacks). Callbacks remaining: {0}",
                         on_erase.callbacks().size())
        m_vector.clear();
    }

    void reserve(const std::size_t n)
    {
        m_vector.reserve(n);
    }
    void resize(const std::size_t n)
    {
        m_vector.resize(n);
    }

    const std::vector<T> &unwrap() const
    {
        return m_vector;
    }

    std::size_t size() const
    {
        return m_vector.size();
    }

    auto begin()
    {
        return m_vector.begin();
    }
    auto begin() const
    {
        return m_vector.begin();
    }

    auto end()
    {
        return m_vector.end();
    }
    auto end() const
    {
        return m_vector.end();
    }

    auto rbegin()
    {
        return m_vector.rbegin();
    }
    auto rbegin() const
    {
        return m_vector.rbegin();
    }

    auto rend()
    {
        return m_vector.rend();
    }
    auto rend() const
    {
        return m_vector.rend();
    }

  private:
    std::vector<T, Alloc> m_vector;
};
} // namespace kit

#endif