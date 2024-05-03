#pragma once

#include "geo/algorithm/intersection2D.hpp"
#include "kit/memory/scope.hpp"
#include "kit/memory/block_allocator.hpp"
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
  public:
    struct properties;
    struct node;
    using partition = std::vector<T>;

    quad_tree(const std::size_t elements_per_quad = 12, std::uint32_t max_depth = 12, const float min_quad_size = 10.f)
        : m_props(kit::make_scope<properties>(elements_per_quad, max_depth, min_quad_size)), m_root(m_props.get())
    {
    }

    quad_tree(const quad_tree &other)
        : m_props(kit::make_scope<properties>(other.m_props->elements_per_squad, other.m_props->max_depth,
                                              other.m_props->min_quad_size)),
          m_root(m_props.get())
    {
        const auto partitions = other.collect_partitions();
        for (const partition *p : partitions)
            for (const T &element : *p)
                insert_and_grow(element);
    }
    quad_tree(quad_tree &&other) : m_props(std::move(other.m_props)), m_root(std::move(other.m_root))
    {
    }

    quad_tree &operator=(const quad_tree &other)
    {
        m_props = kit::make_scope<properties>(other.m_props->elements_per_squad, other.m_props->max_depth,
                                              other.m_props->min_quad_size);

        m_root = node(m_props.get());
        const auto partitions = other.collect_partitions();
        for (const partition *p : partitions)
            for (const T &element : *p)
                insert_and_grow(element);
        return *this;
    }
    quad_tree &operator=(quad_tree &&other)
    {
        m_props = std::move(other.m_props);
        m_root = std::move(other.m_root);
        return *this;
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
        partitions.reserve(32);
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

    struct node
    {
        node(properties *props = nullptr) : props(props)
        {
            if (props)
                elements.reserve(props->elements_per_quad);
        }

        properties *props;
        partition elements;
        std::array<node *, 4> children = {nullptr, nullptr, nullptr, nullptr};
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

        template <RetCallable<geo::aabb2D, T> BoundGetter> void subdivide(BoundGetter getter)
        {
            partitioned = true;
            if (!children[0])
                for (std::size_t i = 0; i < 4; ++i)
                    children[i] = props->m_allocator.create(props);

            for (node *c : children)
            {
                c->depth = depth + 1;
                c->partitioned = false;
                c->elements.clear();
            }

            const glm::vec2 &mm = aabb.min;
            const glm::vec2 &mx = aabb.max;

            const glm::vec2 mid_point = 0.5f * (mm + mx);

            children[0]->aabb = geo::aabb2D(glm::vec2(mm.x, mid_point.y), glm::vec2(mid_point.x, mx.y));
            children[1]->aabb = geo::aabb2D(mid_point, mx);
            children[2]->aabb = geo::aabb2D(mm, mid_point);
            children[3]->aabb = geo::aabb2D(glm::vec2(mid_point.x, mm.y), glm::vec2(mx.x, mid_point.y));
            for (T &element : elements)
                insert_into_children(element, getter);
            elements.clear();
        }

        template <RetCallable<geo::aabb2D, T> BoundGetter>
        void insert_into_children(const T &element, BoundGetter getter)
        {
            for (node *child : children)
                if (geo::intersects(child->aabb, getter(element)))
                    child->insert(element, getter);
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
                for (node *child : children)
                    child->collect_partitions(partitions);
            else if (!elements.empty())
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
        block_allocator<node> m_allocator;

        properties(const properties &other) = default;
        properties(properties &&other) = default;

        properties &operator=(const properties &other) = default;
        properties &operator=(properties &&other) = default;

        friend struct node;
        friend class quad_tree;
    };

  private:
    kit::scope<properties> m_props;
    node m_root;
};
} // namespace kit