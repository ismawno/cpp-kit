#pragma once

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace kit
{
template <template <typename...> typename C, class... Args> class container_view
{
  public:
    container_view(C<Args...> &container) : m_container(container)
    {
    }
    auto begin() const
    {
        return m_container.begin();
    }
    auto end() const
    {
        return m_container.end();
    }

    auto begin()
    {
        return m_container.begin();
    }
    auto end()
    {
        return m_container.end();
    }

    const auto &operator[](const std::size_t index) const
    {
        return m_container[index];
    }
    auto &operator[](const std::size_t index)
    {
        return m_container[index];
    }
    const C<Args...> &unwrap() const
    {
        return m_container;
    }

  private:
    C<Args...> &m_container;
};

template <typename T> using vector_view = container_view<std::vector, T>;

template <typename K, typename V> using unordered_map_view = container_view<std::unordered_map, K, V>;

template <typename K, typename V> using map_view = container_view<std::map, K, V>;

template <typename T> using unordered_set_view = container_view<std::unordered_set, T>;

template <typename T> using set_view = container_view<std::set, T>;
} // namespace kit
