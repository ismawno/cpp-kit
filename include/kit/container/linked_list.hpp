#pragma once

#include "kit/utility/type_constraints.hpp"
#include "kit/debug/log.hpp"

namespace kit
{
// this container does NOT own the data and must be used with pointers. the data is responsible for holding next and
// previous pointers, so i dont have to store additional data. this helps with fragmentation (this container is designed
// to be used with a smart allocator, such as a block allocator). iterators are completely symbolic and are used as a
// tool to manipulate pointers

// the safest way to use this container is that each instance of a linked list has a unique accessor, so that multiple
// instances do not affect the same data. if there are multiple instances, then one has to take extra care to not to
// intertwine both lists. hopefully the debug checks ive added will flag any early bugs

template <typename LA, typename T>
concept LinkAccessor = requires(T *t) {
    {
        LA::next(t)
    } -> std::convertible_to<T *&>;
    {
        LA::prev(t)
    } -> std::convertible_to<T *&>;
};

template <typename T, LinkAccessor<T> LA> class linked_list
{
  public:
    class iterator
    {
      public:
        iterator() = default;
        iterator(T *ptr) : m_ptr(ptr)
        {
        }

        T *operator->() const
        {
            return m_ptr;
        }
        T *operator*() const
        {
            return m_ptr;
        }

        iterator &operator++()
        {
            KIT_ASSERT_ERROR(m_ptr, "Cannot increment null iterator");
            m_ptr = LA::next(m_ptr);
            return *this;
        }
        iterator operator++(int)
        {
            KIT_ASSERT_ERROR(m_ptr, "Cannot increment null iterator");
            const iterator tmp = *this;
            m_ptr = LA::next(m_ptr);
            return tmp;
        }

        iterator &operator--()
        {
            KIT_ASSERT_ERROR(m_ptr, "Cannot decrement null iterator");
            m_ptr = LA::prev(m_ptr);
            return *this;
        }
        iterator operator--(int)
        {
            KIT_ASSERT_ERROR(m_ptr, "Cannot decrement null iterator");
            const iterator tmp = *this;
            m_ptr = LA::prev(m_ptr);
            return tmp;
        }

        bool operator==(const iterator &other) const
        {
            return m_ptr == other.m_ptr;
        }
        bool operator!=(const iterator &other) const
        {
            return m_ptr != other.m_ptr;
        }

        operator bool() const
        {
            return m_ptr != nullptr;
        }

        bool has_next() const
        {
            return LA::next(m_ptr) != nullptr;
        }
        bool has_prev() const
        {
            return LA::prev(m_ptr) != nullptr;
        }

        bool linked() const
        {
            return has_next() || has_prev();
        }

        iterator next() const
        {
            return iterator{LA::next(m_ptr)};
        }
        iterator prev() const
        {
            return iterator{LA::prev(m_ptr)};
        }

      private:
        void link_as_next(iterator it)
        {
            KIT_ASSERT_ERROR(it.m_ptr, "The iterator to append must not be null");
            KIT_ASSERT_ERROR(it.m_ptr != m_ptr, "Cannot append an iterator to itself");
            KIT_ASSERT_ERROR(m_ptr, "Cannot append to a null iterator");
            LA::next(m_ptr) = it.m_ptr;
            LA::prev(it.m_ptr) = m_ptr;
        }
        void link_as_prev(iterator it)
        {
            KIT_ASSERT_ERROR(it.m_ptr, "The iterator to append must not be null");
            KIT_ASSERT_ERROR(it.m_ptr != m_ptr, "Cannot append an iterator to itself");
            KIT_ASSERT_ERROR(m_ptr, "Cannot append to a null iterator");
            LA::prev(m_ptr) = it.m_ptr;
            LA::next(it.m_ptr) = m_ptr;
        }

        // removes a node and reconnects ends. the connection is not severed
        void unlink()
        {
            auto &next = LA::next(m_ptr);
            auto &prev = LA::prev(m_ptr);
            if (next)
                LA::prev(next) = prev;
            if (prev)
                LA::next(prev) = next;

            next = nullptr;
            prev = nullptr;
        }

        T *m_ptr = nullptr;

        friend class linked_list;
    };

    linked_list() = default;

    iterator make_iter(T *ptr)
    {
        return iterator(ptr);
    }

    void insert(iterator pos, T *ptr)
    {
        iterator it = make_iter(ptr);
        KIT_ASSERT_ERROR(!it.linked(), "Cannot append an element that is already linked")
        if (empty())
        {
            m_head = it;
            m_tail = it;
            m_size = 1;
            return;
        }
        KIT_ASSERT_ERROR(!find(ptr), "Cannot append an element that is already in the list")
        KIT_ASSERT_ERROR(pos == end() || find(*pos), "The position iterator must be in the list")

        if (pos == end())
        {
            m_tail.link_as_next(it);
            m_tail = it;
            ++m_size;
            return;
        }
        if (pos == m_head)
        {
            m_head.link_as_prev(it);
            m_head = it;
            ++m_size;
            return;
        }

        iterator prev = pos.prev();
        prev.link_as_next(it);
        pos.link_as_prev(it);
    }

    void push_back(T *ptr)
    {
        insert(end(), ptr);
    }
    void push_front(T *ptr)
    {
        insert(m_head, ptr);
    }

    // after this call, the sublist will be in an invalid state and must not be used
    // i could invalidate it, but i dont want to make it more complicated
    void insert(iterator pos, linked_list &&list)
    {
        if (empty())
        {
            m_head = list.m_head;
            m_tail = list.m_tail;
            m_size = list.m_size;
            return;
        }
        KIT_ASSERT_ERROR(pos == end() || find(*pos), "The position iterator must be in the list")

        if (pos == end())
        {
            m_tail.link_as_next(list.m_head);
            m_tail = list.m_tail;
            m_size += list.m_size;
            return;
        }
        else if (pos == m_head)
        {
            m_head.link_as_prev(list.m_tail);
            m_head = list.m_head;
            m_size += list.m_size;
            return;
        }

        iterator prev = pos.prev();
        prev.link_as_next(list.m_head);
        pos.link_as_prev(list.m_tail);
        m_size += list.m_size;
    }

    void push_back(linked_list &&list)
    {
        insert(end(), std::move(list));
    }
    void push_front(linked_list &&list)
    {
        insert(m_head, std::move(list));
    }

    iterator erase(iterator it)
    {
        if (empty())
            return s_null_iterator;
        KIT_ASSERT_ERROR(
            find(*it), "Cannot erase an element that is not in the list. You may end up unlinking it from another list")

        iterator next = it.next();
        if (it == m_head)
            ++m_head;
        if (it == m_tail)
            --m_tail;
        it.unlink();
        --m_size;
        return next;
    }
    iterator erase(T *ptr)
    {
        return erase(make_iter(ptr));
    }

    iterator pop_back()
    {
        return erase(m_tail);
    }
    iterator pop_front()
    {
        return erase(m_head);
    }

    iterator head() const
    {
        return m_head;
    }
    iterator tail() const
    {
        return m_tail;
    }

    iterator begin() const
    {
        return m_head;
    }
    iterator end() const
    {
        return s_null_iterator;
    }

    // the user will need to decrement the iterator
    iterator rbegin() const
    {
        return m_tail;
    }
    iterator rend() const
    {
        return s_null_iterator;
    }

    std::size_t size() const
    {
        return m_size;
    }
    bool empty() const
    {
        return m_size == 0;
    }
    // to be called when the list has been inserted into another list
    void cleanup()
    {
        m_head = s_null_iterator;
        m_tail = s_null_iterator;
        m_size = 0;
    }

    static iterator find(iterator it1, iterator it2, const T *ptr)
    {
        for (iterator it = it1; it != it2; ++it)
            if (*it == ptr)
                return it;
        return s_null_iterator;
    }

    iterator find(const T *ptr) const
    {
        return find(begin(), end(), ptr);
    }

  private:
    iterator m_head = s_null_iterator;
    iterator m_tail = s_null_iterator;

    std::size_t m_size = 0;

    static inline iterator s_null_iterator{};
};
} // namespace kit