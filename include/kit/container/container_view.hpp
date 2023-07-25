#ifndef KIT_CONTAINER_VIEW_HPP
#define KIT_CONTAINER_VIEW_HPP

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace kit
{
template <template <typename...> typename C, typename... Args> class container_view
{
  public:
    container_view(C<Args...> &vec) : m_vec(vec)
    {
    }
    auto begin() const
    {
        return m_vec.begin();
    }
    auto begin()
    {
        return m_vec.begin();
    }
    auto end() const
    {
        return m_vec.end();
    }
    auto end()
    {
        return m_vec.end();
    }

    const auto &operator[](const std::size_t index) const
    {
        return m_vec[index];
    }
    auto &operator[](const std::size_t index)
    {
        return m_vec[index];
    }
    const C<Args...> &unwrap() const
    {
        return m_vec;
    }

  private:
    C<Args...> &m_vec;
};

template <typename T> using vector_view = container_view<std::vector, T>;

template <typename K, typename V> using unordered_map_view = container_view<std::unordered_map, K, V>;

template <typename K, typename V> using map_view = container_view<std::map, K, V>;

template <typename T> using unordered_set_view = container_view<std::unordered_set, T>;

template <typename T> using set_view = container_view<std::set, T>;
} // namespace kit

#endif