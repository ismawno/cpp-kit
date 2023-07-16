#ifndef KIT_TRACK_VECTOR_HPP
#define KIT_TRACK_VECTOR_HPP

#include "kit/utility/event.hpp"

#include <vector>

namespace kit
{
template <typename T, typename Alloc = std::allocator<T>> class track_vector
{
    // static_assert(std::is_base_of<T, identifiable>::value && std::is_base_of<T, indexable>::value,
    //               "Elements of a track vector must be identifiable and indexable");

  public:
    template <typename... VectorArgs> track_vector(VectorArgs &&...args) : m_vector(std::forward<VectorArgs>(args)...)
    {
    }
    track_vector(std::initializer_list<T> lst) : m_vector(lst)
    {
    }
    event<std::size_t> on_erase;

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
#ifdef KIT_LOG
        T &elm = m_vector.emplace_back(std::forward<EmplaceArgs>(args)...);
        KIT_INFO("Pointer callbacks count: {0}", on_erase.callbacks().size())
        return elm;
#else
        return m_vector.emplace_back(std::forward<EmplaceArgs>(args)...);
#endif
    }
    template <class... PushArgs> void push_back(PushArgs &&...args)
    {
        m_vector.push_back(std::forward<PushArgs>(args)...);
        KIT_INFO("Pointer callbacks count: {0}", on_erase.callbacks().size())
    }

    void erase(std::size_t index)
    {
        m_vector.erase(m_vector.begin() + (long)index);
        on_erase(std::move(index));
        KIT_INFO("Pointer callbacks count: {0}", on_erase.callbacks().size())
    }

    void reserve(const std::size_t n)
    {
        m_vector.reserve(n);
    }
    void resize(const std::size_t n)
    {
        m_vector.resize(n);
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