#pragma once

#include "geo/algorithm/intersection2D.hpp"
#include "kit/memory/ptr/scope.hpp"
#include "kit/memory/allocator/block_allocator.hpp"
#include "kit/utility/type_constraints.hpp"
#include "kit/debug/log.hpp"
#include "kit/profiling/perf.hpp"

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
        properties(const std::size_t elements_per_quad, const std::uint32_t max_depth, const float min_quad_size)
            : elements_per_quad(elements_per_quad), max_depth(max_depth), min_quad_size(min_quad_size)
        {
        }

        std::size_t elements_per_quad;
        std::uint32_t max_depth;
        float min_quad_size;
        bool force_square_shape = true;
    };

    class node
    {
      public:
        bool insert(const T &element, const geo::aabb2D &aabb)
        {
            if (!geo::intersects(m_aabb, aabb))
                return false;
            if (m_leaf && m_elements.size() >= m_props->elements_per_quad && can_subdivide())
                subdivide();
            if (m_leaf)
                m_elements.push_back(element);
            else
                insert_into_children(element);
            return true;
        }
        bool erase(const T &element, const geo::aabb2D &aabb)
        {
            if (m_leaf)
            {
                for (auto it = m_elements.begin(); it != m_elements.end(); ++it)
                    if (*it == element)
                    {
                        m_elements.erase(it);
                        return true;
                    }
                return false;
            }

            bool erased = false;
            for (node *child : m_children)
                if (geo::intersects(child->m_aabb, aabb))
                    erased |= child->erase(element, aabb);
            return erased;
        }

        template <kit::RetCallable<bool, const T> F> void traverse(F &&fun) const
        {
            if (m_leaf)
            {
                for (const T &element : m_elements)
                    if (!fun(element))
                        return;
            }
            else
                for (node *child : m_children)
                    child->traverse(fun);
        }
        template <kit::RetCallable<bool, T> F> void traverse(F &&fun)
        {
            if (m_leaf)
            {
                for (T &element : m_elements)
                    if (!fun(element))
                        return;
            }
            else
                for (node *child : m_children)
                    child->traverse(fun);
        }

        template <kit::RetCallable<bool, const T> F> void traverse(F &&fun, const geo::aabb2D &aabb) const
        {
            if (m_leaf)
            {
                for (const T &element : m_elements)
                    if (!fun(element))
                        return;
            }
            else
                for (node *child : m_children)
                    if (geo::intersects(child->m_aabb, aabb))
                        child->traverse(fun);
        }
        template <kit::RetCallable<bool, T> F> void traverse(F &&fun, const geo::aabb2D &aabb)
        {
            if (m_leaf)
            {
                for (T &element : m_elements)
                    if (!fun(element))
                        return;
            }
            else
                for (node *child : m_children)
                    if (geo::intersects(child->m_aabb, aabb))
                        child->traverse(fun);
        }

        const std::vector<T> &elements() const
        {
            KIT_ASSERT_ERROR(!m_leaf, "Can only access elements from a leaf node")
            return m_elements;
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
        node(properties *props = nullptr) : m_props(props)
        {
            if (props)
                m_elements.reserve(props->elements_per_quad);
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
                c->m_elements.clear();
            }

            const glm::vec2 &mm = m_aabb.min;
            const glm::vec2 &mx = m_aabb.max;

            const glm::vec2 mid_point = 0.5f * (mm + mx);

            m_children[0]->m_aabb = geo::aabb2D(glm::vec2(mm.x, mid_point.y), glm::vec2(mid_point.x, mx.y));
            m_children[1]->m_aabb = geo::aabb2D(mid_point, mx);
            m_children[2]->m_aabb = geo::aabb2D(mm, mid_point);
            m_children[3]->m_aabb = geo::aabb2D(glm::vec2(mid_point.x, mm.y), glm::vec2(mx.x, mid_point.y));
            for (const T &element : m_elements)
                insert_into_children(element);
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
        std::vector<T> m_elements;
        std::array<node *, 4> m_children = {nullptr, nullptr, nullptr, nullptr};
        geo::aabb2D m_aabb;

        std::uint32_t m_depth = 0;
        bool m_leaf = false;
    };

    quad_tree(const std::size_t elements_per_quad = 8, std::uint32_t max_depth = 12, const float min_quad_size = 10.f)
        : m_props(kit::make_scope<properties>(elements_per_quad, max_depth, min_quad_size)), m_root(m_props.get())
    {
    }

    quad_tree &operator=(quad_tree &&) = default;
    quad_tree(quad_tree &&) = default;

    void insert(const T &element, const geo::aabb2D &aabb)
    {
        KIT_ASSERT_ERROR(geo::contains(m_root.m_aabb, aabb),
                         "Element aabb must be contained within the quad tree bounds")
        m_root.insert(element, aabb);
    }

    bool erase(const T &element, const geo::aabb2D &aabb)
    {
        KIT_ASSERT_ERROR(geo::contains(m_root.m_aabb, aabb),
                         "Element aabb must be contained within the quad tree bounds")
        return m_root.erase(element, aabb);
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
        KIT_ASSERT_ERROR(geo::contains(m_root.m_aabb, aabb),
                         "Traversal aabb must be contained within the quad tree bounds")
        m_root.traverse(fun, aabb);
    }
    template <kit::RetCallable<bool, T> F> void traverse(F &&fun, const geo::aabb2D &aabb)
    {
        KIT_ASSERT_ERROR(geo::contains(m_root.m_aabb, aabb),
                         "Traversal aabb must be contained within the quad tree bounds")
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