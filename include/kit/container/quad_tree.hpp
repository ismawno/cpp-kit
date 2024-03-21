#pragma once

#include "geo/algorithm/intersection.hpp"
#include "kit/memory/scope.hpp"
#include "kit/utility/type_constraints.hpp"
#include "kit/debug/log.hpp"

#include <glm/vec2.hpp>
#include <vector>
#include <array>
#include <stack>

namespace kit
{
template <typename T> class quad_tree
{
    struct properties;
    struct node;

  public:
    using partition = std::vector<T>;

    quad_tree(const std::size_t elements_per_quad = 12, const std::uint32_t max_depth = 12,
              const float min_quad_size = 14.f)
        : m_props(kit::make_scope<properties>(elements_per_quad, max_depth, min_quad_size)), m_root(m_props.get())
    {
    }

    template <RetCallable<geo::aabb2D, T> BoundGetter> bool insert(const T &element, BoundGetter getter)
    {
        const geo::aabb2D &bounds = getter(element);
        if (!geo::intersects(m_root.aabb, bounds))
            return false;
        m_root.insert(element, getter);
        return true;
    }
    template <RetCallable<geo::aabb2D, T> BoundGetter> void insert_and_grow(const T &element, BoundGetter getter)
    {
        const geo::aabb2D &bounds = getter(element);
        m_root.aabb += bounds;
        m_root.insert(element, getter);
    }
    void clear()
    {
        m_root.elements.clear();
        m_root.partitioned = false;
    }
    bool empty() const
    {
        return m_root.elements.empty() && !m_root.partitioned;
    }

    std::vector<const partition *> collect_partitions() const
    {
        std::vector<const partition *> partitions;
        partitions.reserve(m_props->m_nodes.size());
        m_root.collect_partitions(partitions);
        return partitions;
    }

    const geo::aabb2D &bounds() const
    {
        return m_root.aabb;
    }
    void bounds(const geo::aabb2D &bounds)
    {
        m_root.aabb = bounds;
    }

    const node &root() const
    {
        return m_root;
    }
    const properties &props() const
    {
        return *m_props;
    }
    properties &props()
    {
        return *m_props;
    }

    quad_tree(const quad_tree &other)
    {
        m_props = kit::make_scope<properties>(*other.m_props);
    }
    quad_tree(quad_tree &&other) noexcept
    {
        m_props = std::move(other.m_props);
    }

    quad_tree &operator=(const quad_tree &other)
    {
        m_props = kit::make_scope<properties>(*other.m_props);
        return *this;
    }
    quad_tree &operator=(quad_tree &&other) noexcept
    {
        m_props = std::move(other.m_props);
        return *this;
    }

  private:
    struct node
    {
        node(properties *props) : props(props)
        {
            elements.reserve(props->elements_per_quad);
        }

        properties *props;
        partition elements;
        std::array<std::size_t, 4> children = {SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX};
        geo::aabb2D aabb;

        std::uint32_t depth = 0;
        bool partitioned = false;

        template <RetCallable<geo::aabb2D, T> BoundGetter> void insert(const T &element, BoundGetter getter)
        {
            KIT_ASSERT_ERROR(geo::intersects(aabb, getter(element)), "Element is not within the bounds of the quad")
            if (elements.size() >= props->elements_per_quad && can_subdivide())
                subdivide(getter);
            if (partitioned)
                insert_into_children(element, getter);
            else
                elements.push_back(element);
        }

        std::array<const node *, 4> quads() const
        {
            KIT_ASSERT_ERROR(partitioned, "Node is not partitioned")
            std::array<const node *, 4> cinstances = {&props->m_nodes[children[0]], &props->m_nodes[children[1]],
                                                      &props->m_nodes[children[2]], &props->m_nodes[children[3]]};
            return cinstances;
        }
        std::array<node *, 4> quads()
        {
            KIT_ASSERT_ERROR(partitioned, "Node is not partitioned")
            std::array<node *, 4> cinstances = {&props->m_nodes[children[0]], &props->m_nodes[children[1]],
                                                &props->m_nodes[children[2]], &props->m_nodes[children[3]]};
            return cinstances;
        }

        template <RetCallable<geo::aabb2D, T> BoundGetter> void subdivide(BoundGetter getter)
        {
            partitioned = true;
            if (children[0] == SIZE_MAX)
                for (std::size_t i = 0; i < 4; ++i)
                {
                    children[i] = props->m_nodes.size();
                    props->m_nodes.emplace_back(props);
                }
            std::array<node *, 4> cinstances = quads();
            for (node *c : cinstances)
            {
                c->depth = depth + 1;
                c->partitioned = false;
                c->elements.clear();
            }

            const glm::vec2 &mm = aabb.min, &mx = aabb.max;
            const glm::vec2 mid_point = 0.5f * (mm + mx), hdim = 0.5f * (mx - mm);
            cinstances[0]->aabb = geo::aabb2D(glm::vec2(mm.x, mm.y + hdim.y), glm::vec2(mx.x - hdim.x, mx.y));
            cinstances[1]->aabb = geo::aabb2D(mid_point, mx);
            cinstances[2]->aabb = geo::aabb2D(mm, mid_point);
            cinstances[3]->aabb = geo::aabb2D(glm::vec2(mm.x + hdim.x, mm.y), glm::vec2(mx.x, mx.y - hdim.y));
            for (T &element : elements)
                insert_into_children(element, getter);
            elements.clear();
        }

        template <RetCallable<geo::aabb2D, T> BoundGetter>
        void insert_into_children(const T &element, BoundGetter getter)
        {
            for (std::size_t index : children)
                if (geo::intersects(props->m_nodes[index].aabb, getter(element)))
                    props->m_nodes[index].insert(element, getter);
        }

        bool can_subdivide() const
        {
            if (depth >= props->max_depth)
                return false;
            const glm::vec2 dim = aabb.dimension();
            return dim.x * dim.y >= props->min_quad_size * props->min_quad_size;
        }

        void collect_partitions(std::vector<const partition *> &partitions) const
        {
            if (partitioned)
                for (std::size_t index : children)
                    props->m_nodes[index].collect_partitions(partitions);
            else
                partitions.push_back(&elements);
        }
    };

    struct properties
    {
        properties(const std::size_t elements_per_quad, const std::uint32_t max_depth, const float min_quad_size)
            : elements_per_quad(elements_per_quad), max_depth(max_depth), min_quad_size(min_quad_size)
        {
        }

        std::size_t elements_per_quad;
        std::uint32_t max_depth;
        float min_quad_size;

      private:
        std::vector<node> m_nodes;

        properties(const properties &other) = default;
        properties(properties &&other) = default;

        properties &operator=(const properties &other) = default;
        properties &operator=(properties &&other) = default;

        friend struct node;
        friend class quad_tree;
    };

    kit::scope<properties> m_props;
    node m_root;
};
} // namespace kit