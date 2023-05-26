#ifndef CONTAINER_VIEW_HPP
#define CONTAINER_VIEW_HPP

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cvw
{
template <template <typename...> typename C, typename... Args> class container
{
  public:
    container(C<Args...> &vec) : m_vec(vec)
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

template <typename T> using vector = container<std::vector, T>;

template <typename K, typename V> using unordered_map = container<std::unordered_map, K, V>;

template <typename K, typename V> using map = container<std::map, K, V>;

template <typename T> using unordered_set = container<std::unordered_set, T>;

template <typename T> using set = container<std::set, T>;
} // namespace cvw

#endif