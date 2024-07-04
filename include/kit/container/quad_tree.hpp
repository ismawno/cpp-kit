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
template <typename T>
concept QuadTreeElement = requires(T t) {
    {
        t()
    } -> std::convertible_to<geo::aabb2D>;
    {
        t == t
    } -> std::convertible_to<bool>;
    {
        std::hash<T>{}(t)
    } -> std::convertible_to<std::size_t>;
};

template <QuadTreeElement T, template <typename> class Allocator = block_allocator> class quad_tree
{
  public:
    struct partition
    {
        using to_compare_t = std::vector<T>;
        const std::vector<T> *elements;
        to_compare_t to_compare;
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
        bool force_square_shape = true;
    };

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
            insert(elem);
    }
    quad_tree(quad_tree &&other)
        : m_props(std::move(other.m_props)), m_root(std::move(other.m_root)),
          m_element_bounds(std::move(other.m_element_bounds))
    {
    }

    quad_tree &operator=(const quad_tree &other)
    {
        m_props = kit::make_scope<properties>(other.m_props->elements_per_squad, other.m_props->max_depth,
                                              other.m_props->min_quad_size);

        m_root = node(m_props.get());
        const auto elements = other.collect_elements();
        for (const auto &elem : elements)
            insert(elem);
        return *this;
    }
    quad_tree &operator=(quad_tree &&other)
    {
        m_props = std::move(other.m_props);
        m_root = std::move(other.m_root);
        m_element_bounds = std::move(other.m_element_bounds);
        return *this;
    }

    void insert(const T &element)
    {
        KIT_PERF_FUNCTION()
        KIT_ASSERT_ERROR(!m_element_bounds.contains(element), "Element already exists in the quad tree")
        const geo::aabb2D &bounds = element();
        m_element_bounds.emplace(element, bounds);

        m_root.aabb += bounds;
        m_root.insert(element);
    }

    bool erase(const T &element)
    {
        KIT_PERF_FUNCTION()
        const auto it = m_element_bounds.find(element);
        if (it == m_element_bounds.end())
            return false;
        const bool found = m_root.erase(element, it->second);
        KIT_ASSERT_ERROR(found, "Present element was not found in the quad tree")
        m_element_bounds.erase(it);
        return found;
    }

    void clear()
    {
        KIT_PERF_FUNCTION()
        m_element_bounds.clear();
        m_root.clear();
    }
    std::size_t size() const
    {
        return m_root.size();
    }
    bool empty() const
    {
        return m_root.empty();
    }
    bool checksum() const
    {
        return m_element_bounds.size() == m_root.size() && m_element_bounds.empty() == empty();
    }

    std::vector<partition> collect_partitions() const
    {
        KIT_PERF_FUNCTION()
        std::vector<partition> partitions;
        partitions.reserve(8);
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

    void rebuild()
    {
        const auto elements = collect_elements();
        clear();
        for (const auto &elem : elements)
            insert(elem);
    }

    const geo::aabb2D &bounds() const
    {
        return m_root.aabb;
    }
    void bounds(const geo::aabb2D &bounds)
    {
        KIT_ASSERT_ERROR(empty(), "Cannot change bounds of a non-empty quad tree")
        m_root.aabb = bounds;
    }
    void rebound_and_rebuild(const geo::aabb2D &bounds)
    {
        m_root.aabb = bounds;
        rebuild();
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
        *m_props = props;
        rebuild();
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
        std::size_t children_size = 0;

        bool empty() const
        {
            return elements.empty() && children_size == 0;
        }
        bool partitioned() const
        {
            return children_size != 0;
        }
        std::size_t size() const
        {
            return elements.size() + children_size;
        }
        void clear()
        {
            elements.clear();
            children_size = 0;
            if (partitioned())
                for (node *child : children)
                    child->clear();
        }

        void insert(const T &element)
        {
            KIT_ASSERT_ERROR(geo::intersects(aabb, element()), "Element is not within the bounds of the quad")
            if (!partitioned() && elements.size() >= props->elements_per_quad && can_subdivide())
                subdivide();
            if (!partitioned() || !try_insert_into_children(element))
                elements.push_back(element);
        }

        bool erase(const T &element, const geo::aabb2D &ebounds)
        {
            KIT_ASSERT_ERROR(geo::intersects(aabb, ebounds), "Element is not within the bounds of the quad")
            for (auto it = elements.begin(); it != elements.end(); ++it)
                if (*it == element)
                {
                    elements.erase(it);
                    return true;
                }
            if (!partitioned())
                return false;
            for (node *child : children)
                if (geo::intersects(child->aabb, ebounds) && child->erase(element, ebounds))
                {
                    children_size--;
                    return true;
                }
            return false;
        }

        void subdivide()
        {
            if (!children[0])
                for (std::size_t i = 0; i < 4; ++i)
                    children[i] = s_allocator.create(props);

            for (node *c : children)
            {
                c->depth = depth + 1;
                c->elements.clear();
                c->children_size = 0;
            }

            const glm::vec2 &mm = aabb.min;
            const glm::vec2 &mx = aabb.max;

            const glm::vec2 mid_point = 0.5f * (mm + mx);

            children[0]->aabb = geo::aabb2D(glm::vec2(mm.x, mid_point.y), glm::vec2(mid_point.x, mx.y));
            children[1]->aabb = geo::aabb2D(mid_point, mx);
            children[2]->aabb = geo::aabb2D(mm, mid_point);
            children[3]->aabb = geo::aabb2D(glm::vec2(mid_point.x, mm.y), glm::vec2(mx.x, mid_point.y));
            for (auto it = elements.begin(); it != elements.end();)
                if (try_insert_into_children(*it))
                    it = elements.erase(it);
                else
                    ++it;
        }

        bool try_insert_into_children(const T &element)
        {
            node *chosen = nullptr;
            bool multiple = false;
            for (node *child : children)
                if (geo::intersects(child->aabb, element()))
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
            chosen->insert(element);
            children_size++;
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

            if (partitioned())
                for (node *child : children)
                {
                    if (!elements.empty())
                        child->collect_elements(partitions[current_partition].to_compare);
                    child->collect_partitions(partitions);
                }
        }

        void collect_elements(partition::to_compare_t &to_insert) const
        {
            to_insert.insert(to_insert.end(), elements.begin(), elements.end());
            if (partitioned())
                for (node *child : children)
                    child->collect_elements(to_insert);
        }
    };

  private:
    kit::scope<properties> m_props;
    node m_root;
    std::unordered_map<T, geo::aabb2D> m_element_bounds;

    static inline Allocator<node> s_allocator{};
};
} // namespace kit