#pragma once

#include "geo/algorithm/intersection2D.hpp"
#include "kit/memory/ptr/scope.hpp"
#include "kit/memory/allocator/block_allocator.hpp"
#include "kit/utility/type_constraints.hpp"
#include "kit/debug/log.hpp"

#include <glm/vec2.hpp>
#include <vector>
#include <array>
#include <stack>

namespace kit
{
template <typename T, template <typename> class Allocator = block_allocator> class quad_tree
{
  public:
    struct properties
    {
        properties() = default;
        properties(const std::size_t elements_per_quad, const std::uint32_t max_depth, const float min_quad_size)
            : elements_per_quad(elements_per_quad), max_depth(max_depth), min_quad_size(min_quad_size)
        {
        }

        std::size_t elements_per_quad;
        std::uint32_t max_depth;
        float min_quad_size;
        bool force_square_shape = true;
    };
    struct entry
    {
        T element;
        geo::aabb2D aabb;
    };

    class node
    {
      public:
        node(properties *props = nullptr) : m_props(props)
        {
            if (props)
                m_elements.reserve(props->elements_per_quad);
        }

        bool insert(const T &element, const geo::aabb2D &aabb)
        {
            if (!geo::intersects(m_aabb, aabb))
                return false;
            if (m_leaf && m_elements.size() >= m_props->elements_per_quad && can_subdivide())
                subdivide();

            if (m_leaf)
                m_elements.push_back({element, aabb});
            else
                insert_into_children(element, aabb);
            return true;
        }
        bool erase(const T &element, const geo::aabb2D &aabb)
        {
            bool erased = false;
            if (!m_leaf)
                for (node *child : m_children)
                    erased |= child->erase(element, aabb);
            // A child could have triggered a parent merge
            if (m_leaf)
                erased |= erase_as_leaf(element);

            return erased;
        }
        bool erase(const T &element)
        {
            bool erased = false;
            if (!m_leaf)
                for (node *child : m_children)
                    erased |= child->erase(element);
            // A child could have triggered a parent merge
            if (m_leaf)
                erased |= erase_as_leaf(element);

            return erased;
        }

        template <kit::RetCallable<bool, const T> F> void traverse(F &&fun) const
        {
            if (m_leaf)
                traverse_as_leaf<const entry, F>(std::forward<F>(fun));
            else
                for (node *child : m_children)
                    child->traverse(fun);
        }
        template <kit::RetCallable<bool, T> F> void traverse(F &&fun)
        {
            if (m_leaf)
                traverse_as_leaf<entry, F>(std::forward<F>(fun));
            else
                for (node *child : m_children)
                    child->traverse(fun);
        }

        template <kit::RetCallable<bool, const T> F> void traverse(F &&fun, const geo::aabb2D &aabb) const
        {
            if (m_leaf)
                traverse_as_leaf<const entry, F>(std::forward<F>(fun));
            else
                for (node *child : m_children)
                    if (geo::intersects(child->m_aabb, aabb))
                        child->traverse(fun, aabb);
        }
        template <kit::RetCallable<bool, T> F> void traverse(F &&fun, const geo::aabb2D &aabb)
        {
            if (m_leaf)
                traverse_as_leaf<entry, F>(std::forward<F>(fun));
            else
                for (node *child : m_children)
                    if (geo::intersects(child->m_aabb, aabb))
                        child->traverse(fun, aabb);
        }

        const std::vector<entry> &elements() const
        {
            KIT_ASSERT_ERROR(m_leaf, "Can only access elements from a leaf node")
            return m_elements;
        }
        const std::array<node *, 4> &children() const
        {
            KIT_ASSERT_ERROR(!m_leaf, "Can only access children from a non-leaf node")
            return m_children;
        }

        const geo::aabb2D &aabb() const
        {
            return m_aabb;
        }
        bool leaf() const
        {
            return m_leaf;
        }

      private:
        bool erase_as_leaf(const T &element)
        {
            for (auto it = m_elements.begin(); it != m_elements.end(); ++it)
                if (it->element == element)
                {
                    m_elements.erase(it);
                    if (m_parent && m_elements.size() <= m_props->elements_per_quad / 4)
                        m_parent->try_merge();
                    return true;
                }
            return false;
        }

        template <typename U, kit::RetCallable<bool, T> F> void traverse_as_leaf(F &&fun)
        {
            for (U &entry : m_elements)
                if (!std::forward<F>(fun)(entry.element))
                    return;
        }

        void subdivide()
        {
            m_leaf = false;
            if (!m_children[0])
                for (std::size_t i = 0; i < 4; ++i)
                    m_children[i] = s_allocator.create(m_props);

            for (node *c : m_children)
            {
                c->m_depth = m_depth + 1;
                c->m_leaf = true;
                c->m_elements.clear();
                c->m_parent = this;
            }

            const glm::vec2 &mm = m_aabb.min;
            const glm::vec2 &mx = m_aabb.max;

            const glm::vec2 mid_point = 0.5f * (mm + mx);

            m_children[0]->m_aabb = geo::aabb2D(glm::vec2(mm.x, mid_point.y), glm::vec2(mid_point.x, mx.y));
            m_children[1]->m_aabb = geo::aabb2D(mid_point, mx);
            m_children[2]->m_aabb = geo::aabb2D(mm, mid_point);
            m_children[3]->m_aabb = geo::aabb2D(glm::vec2(mid_point.x, mm.y), glm::vec2(mx.x, mid_point.y));
            for (const entry &e : m_elements)
                insert_into_children(e.element, e.aabb);
            m_elements.clear();
        }

        void try_merge()
        {
            std::size_t total_elements = 0;
            for (const node *child : m_children)
            {
                if (!child->m_leaf)
                    return;
                total_elements += child->m_elements.size();
            }
            if (total_elements > m_props->elements_per_quad)
                return;
            for (const node *child : m_children) // painful
                for (const entry &e1 : child->m_elements)
                {
                    bool found = false;
                    for (const entry &e2 : m_elements)
                        if (e1.element == e2.element)
                        {
                            found = true;
                            break;
                        }
                    if (!found)
                        m_elements.push_back(e1);
                }

            m_leaf = true;
        }

        void insert_into_children(const T &element, const geo::aabb2D &aabb)
        {
            for (node *child : m_children)
                child->insert(element, aabb);
        }

        bool can_subdivide() const
        {
            if (m_depth >= m_props->max_depth)
                return false;
            const glm::vec2 dim = m_aabb.dimension();
            return dim.x * dim.y >= m_props->min_quad_size * m_props->min_quad_size;
        }

        properties *m_props;
        std::vector<entry> m_elements;

        node *m_parent = nullptr;
        std::array<node *, 4> m_children = {nullptr, nullptr, nullptr, nullptr};

        geo::aabb2D m_aabb;

        std::uint32_t m_depth = 0;
        bool m_leaf = true;

        friend class quad_tree;
    };

    quad_tree(const std::size_t elements_per_quad = 8, std::uint32_t max_depth = 12, const float min_quad_size = 40.f)
        : m_props(kit::make_scope<properties>(elements_per_quad, max_depth, min_quad_size)), m_root(m_props.get())
    {
    }
    quad_tree(const properties &props) : m_props(kit::make_scope<properties>(props)), m_root(m_props.get())
    {
    }

    quad_tree &operator=(quad_tree &&) = default;
    quad_tree(quad_tree &&) = default;

    bool insert(const T &element, const geo::aabb2D &aabb)
    {
        KIT_ASSERT_WARN(geo::intersects(m_root.m_aabb, aabb),
                        "Element aabb does not intersect with the quad tree bounds")
        return m_root.insert(element, aabb);
    }
    bool erase(const T &element, const geo::aabb2D &aabb)
    {
        KIT_ASSERT_WARN(geo::intersects(m_root.m_aabb, aabb),
                        "Element aabb does not intersect with the quad tree bounds")
        return m_root.erase(element, aabb);
    }
    bool erase(const T &element)
    {
        return m_root.erase(element);
    }

    void clear()
    {
        m_root.m_elements.clear();
        m_root.m_leaf = true;
    }
    bool empty() const
    {
        return m_root.m_elements.empty() && m_root.m_leaf;
    }

    template <kit::RetCallable<bool, const T> F> void traverse(F &&fun) const
    {
        m_root.traverse(fun);
    }
    template <kit::RetCallable<bool, T> F> void traverse(F &&fun)
    {
        m_root.traverse(fun);
    }

    template <kit::RetCallable<bool, const T> F> void traverse(F &&fun, const geo::aabb2D &aabb) const
    {
        KIT_ASSERT_ERROR(geo::intersects(m_root.m_aabb, aabb),
                         "Traversal aabb must intersect with the quad tree bounds")
        m_root.traverse(fun, aabb);
    }
    template <kit::RetCallable<bool, T> F> void traverse(F &&fun, const geo::aabb2D &aabb)
    {
        KIT_ASSERT_ERROR(geo::intersects(m_root.m_aabb, aabb),
                         "Traversal aabb must intersect with the quad tree bounds")
        m_root.traverse(fun, aabb);
    }

    const geo::aabb2D &aabb() const
    {
        return m_root.m_aabb;
    }
    void aabb(const geo::aabb2D &aabb)
    {
        KIT_ASSERT_ERROR(empty(), "Cannot change aabb of a non-empty quad tree")
        m_root.m_aabb = aabb;
    }

    const node &root() const
    {
        return m_root;
    }
    const properties &props() const
    {
        return *m_props;
    }
    void props(const properties &props)
    {
        KIT_ASSERT_ERROR(empty(), "Cannot change properties of a non-empty quad tree")
        *m_props = props;
    }

  private:
    kit::scope<properties> m_props;
    node m_root;

    quad_tree(const quad_tree &) = delete;
    quad_tree &operator=(const quad_tree &) = delete;

    static inline Allocator<node> s_allocator{};
};
} // namespace kit