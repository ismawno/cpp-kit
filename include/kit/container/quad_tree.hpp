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

#define KIT_QT_COLLECT_ELEMENTS_COPY

namespace kit
{
template <typename T, template <typename> class Allocator = block_allocator> class quad_tree
{
  public:
    struct partition
    {
#ifdef KIT_QT_COLLECT_ELEMENTS_COPY
        using to_compare_t = std::vector<T>;
#else
        using to_compare_t = std::vector<const std::vector<T> *>;
#endif

        const std::vector<T> *elements;
        to_compare_t to_compare;
    };

    struct properties;
    struct node;

    quad_tree(const std::size_t elements_per_quad = 8, std::uint32_t max_depth = 12, const float min_quad_size = 10.f)
        : m_props(kit::make_scope<properties>(elements_per_quad, max_depth, min_quad_size)), m_root(m_props.get())
    {
    }

    quad_tree(const quad_tree &other)
        : m_props(kit::make_scope<properties>(other.m_props->elements_per_squad, other.m_props->max_depth,
                                              other.m_props->min_quad_size)),
          m_root(m_props.get())
    {
        const auto elements = other.collect_elements();
        for (const auto &elem : elements)
#ifdef KIT_QT_COLLECT_ELEMENTS_COPY
            insert_and_grow(elem);
#else
            for (const auto &e : *elem)
                insert_and_grow(e);
#endif
    }
    quad_tree(quad_tree &&other) : m_props(std::move(other.m_props)), m_root(std::move(other.m_root))
    {
    }

    quad_tree &operator=(const quad_tree &other)
    {
        m_props = kit::make_scope<properties>(other.m_props->elements_per_squad, other.m_props->max_depth,
                                              other.m_props->min_quad_size);

        m_root = node(m_props.get());
        const auto elements = other.collect_elements();
        for (const auto &elem : elements)
#ifdef KIT_QT_COLLECT_ELEMENTS_COPY
            insert_and_grow(elem);
#else
            for (const auto &e : *elem)
                insert_and_grow(e);
#endif
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
        KIT_PERF_FUNCTION()
        const geo::aabb2D &bounds = getter(element);
        if (!geo::intersects(m_root.aabb, bounds))
            return false;
        m_root.insert(element, getter);
        return true;
    }
    template <RetCallable<geo::aabb2D, T> BoundGetter> void insert_and_grow(const T &element, BoundGetter getter)
    {
        KIT_PERF_FUNCTION()
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

    std::vector<partition> collect_partitions() const
    {
        KIT_PERF_FUNCTION()
        std::vector<partition> partitions;
        partitions.reserve(32);
        m_root.collect_partitions(partitions);
        return partitions;
    }
    std::vector<T> collect_elements() const
    {
        KIT_PERF_FUNCTION()
        std::vector<T> elements;
        elements.reserve(32);
        m_root.collect_elements(elements);
        return elements;
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
        std::vector<T> elements;
        std::array<node *, 4> children = {nullptr, nullptr, nullptr, nullptr};
        geo::aabb2D aabb;

        std::uint32_t depth = 0;
        bool partitioned = false;

        template <RetCallable<geo::aabb2D, T> BoundGetter> void insert(const T &element, BoundGetter getter)
        {
            KIT_ASSERT_ERROR(geo::intersects(aabb, getter(element)), "Element is not within the bounds of the quad")
            if (!partitioned && elements.size() >= props->elements_per_quad && can_subdivide())
                subdivide(getter);
            if (!partitioned || !try_insert_into_children(element, getter))
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
            for (auto it = elements.begin(); it != elements.end();)
                if (try_insert_into_children(*it, getter))
                    it = elements.erase(it);
                else
                    ++it;
        }

        template <RetCallable<geo::aabb2D, T> BoundGetter>
        bool try_insert_into_children(const T &element, BoundGetter getter)
        {
            node *chosen = nullptr;
            bool multiple = false;
            for (node *child : children)
                if (geo::intersects(child->aabb, getter(element)))
                {
                    if (chosen)
                    {
                        multiple = true;
                        break;
                    }
                    chosen = child;
                }
            if (multiple)
                return false;
            chosen->insert(element, getter);
            return true;
        }

        bool can_subdivide() const
        {
            if (depth >= props->max_depth)
                return false;
            const glm::vec2 dim = aabb.dimension();
            return dim.x * dim.y >= props->min_quad_size * props->min_quad_size;
        }

        void collect_partitions(std::vector<partition> &partitions) const
        {
            std::size_t current_partition;
            if (!elements.empty())
            {
                current_partition = partitions.size();
                partition &p = partitions.emplace_back();
                p.elements = &elements;
                p.to_compare.reserve(8);
            }

            if (partitioned)
                for (node *child : children)
                {
                    if (!elements.empty())
                        child->collect_elements(partitions[current_partition].to_compare);
                    child->collect_partitions(partitions);
                }
        }

        void collect_elements(partition::to_compare_t &to_insert) const
        {
#ifdef KIT_QT_COLLECT_ELEMENTS_COPY
            to_insert.insert(to_insert.end(), elements.begin(), elements.end());
#else
            if (!elements.empty())
                to_insert.push_back(&elements);
#endif
            if (partitioned)
                for (node *child : children)
                    child->collect_elements(to_insert);
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
        Allocator<node> m_allocator;

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