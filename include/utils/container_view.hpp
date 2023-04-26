#ifndef CONTAINER_VIEW_HPP
#define CONTAINER_VIEW_HPP

#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>

namespace utils
{
    template <typename T>
    class container_view
    {
    public:
        container_view(T &vec) : m_vec(vec) {}
        auto begin() const { return m_vec.begin(); }
        auto begin() { return m_vec.begin(); }
        auto end() const { return m_vec.end(); }
        auto end() { return m_vec.end(); }

        const auto &operator[](const std::size_t index) const { return m_vec[index]; }
        auto &operator[](const std::size_t index) { return m_vec[index]; }
        const T &unwrap() const { return m_vec; }

    private:
        T &m_vec;
    };

    template <typename T>
    using vector_view = container_view<std::vector<T>>;

    template <typename K, typename V>
    using umap_view = container_view<std::unordered_map<K, V>>;

    template <typename K, typename V>
    using map_view = container_view<std::map<K, V>>;

    template <typename T>
    using uset_view = container_view<std::unordered_set<T>>;

    template <typename T>
    using set_view = container_view<std::set<T>>;
}

#endif