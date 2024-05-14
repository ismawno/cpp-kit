#pragma once

#include "kit/memory/allocator/allocator.hpp"
#include "kit/container/dynarray.hpp"
#include <vector>

namespace kit
{
template <typename T, std::size_t Capacity = 0> class stack_allocator final : public continuous_allocator<T>
{
  public:
    T *nallocate(const std::size_t count) override
    {
        KIT_ASSERT_ERROR(count > 0, "Cannot allocate zero elements");
        const std::size_t size = count * sizeof(T);
        KIT_ASSERT_ERROR(Capacity * sizeof(T) - m_memory.size() >= size,
                         "Out of memory! Requested {0} bytes, but only {1} bytes are available", size,
                         Capacity * sizeof(T) - m_memory.size());

        T *ptr = (T *)(m_memory.data() + m_memory.size());
        m_memory.resize(m_memory.size() + size);
        m_entries.push_back({ptr, size});
        return ptr;
    }
    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(!m_memory.empty(), "Cannot deallocate from an empty stack allocator");
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        KIT_ASSERT_ERROR(can_deallocate(ptr), "A stack allocator can only deallocate the last allocated object");
        m_memory.resize(m_memory.size() - m_entries.back());
        m_entries.pop_back();
    }

    bool owns(const T *ptr) const override
    {
        return ptr >= m_memory.data() && ptr < m_memory.data() + m_memory.size();
    }
    bool can_deallocate(const T *ptr) const
    {
        return ptr == m_entries.back().ptr;
    }
    std::size_t remaining_slots() const
    {
        return Capacity - m_memory.size() / sizeof(T);
    }
    std::size_t remaining_size() const
    {
        return Capacity * sizeof(T) - m_memory.size();
    }

  private:
    struct entry
    {
        T *ptr;
        std::size_t size;
    };
    dynarray<std::byte, Capacity> m_memory;
    dynarray<entry, Capacity> m_entries;
};

template <typename T> class stack_allocator<T, 0> final : public continuous_allocator<T>
{
  public:
    stack_allocator(const std::size_t stack_obj_count) : m_stack_obj_count(stack_obj_count)
    {
    }

    T *nallocate(const std::size_t count) override
    {
        KIT_ASSERT_ERROR(count > 0, "Cannot allocate zero elements");
        KIT_ASSERT_ERROR(m_stack_obj_count >= count,
                         "Out of memory! Requested {0} objects, but only {1} slots are available per stack", count,
                         m_stack_obj_count);

        if (m_stacks.empty() || count > m_stack_obj_count - m_current_stack_count)
            return first_element_of_new_stack(count);
        return next_free_element_of_current_stack(count);
    }
    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(!m_stacks.empty(), "Cannot deallocate from an empty stack allocator");
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        KIT_ASSERT_ERROR(can_deallocate(ptr), "A stack allocator can only deallocate the last allocated object");
        if (m_current_stack_count == 0)
        {
            m_current_stack--;
            m_current_stack_count = m_entries.back().stack_count;
        }
        else
            m_current_stack_count -= m_entries.back().alloc_count;
        m_entries.pop_back();
    }

    bool owns(const T *ptr) const override
    {
        for (T *stack : m_stacks)
            if (ptr >= stack && ptr < stack + m_stack_obj_count)
                return true;
        return false;
    }
    bool can_deallocate(const T *ptr) const
    {
        return ptr == m_entries.back().ptr;
    }

  private:
    T *first_element_of_new_stack(const std::size_t count)
    {
        constexpr std::size_t align = alignof(T);
        const std::size_t size = continuous_allocator<T>::aligned_size(m_stack_obj_count * sizeof(T), align);
        T *stack = (T *)continuous_allocator<T>::platform_aware_aligned_alloc(size, align);

        m_current_stack = m_stacks.size();
        m_stacks.push_back(stack);
        m_current_stack_count = count;
        m_entries.push_back({stack, count, count});
        return stack;
    }

    T *next_free_element_of_current_stack(const std::size_t count)
    {
        T *ptr = m_stacks[m_current_stack] + m_current_stack_count;
        m_current_stack_count += count;
        m_entries.push_back({ptr, count, m_current_stack_count});
        return ptr;
    }

  private:
    struct entry
    {
        T *ptr;
        std::size_t alloc_count;
        std::size_t stack_count;
    };

    std::vector<T *> m_stacks;
    std::vector<entry> m_entries;
    std::size_t m_stack_obj_count;
    std::size_t m_current_stack;
    std::size_t m_current_stack_count = 0;
};

template <std::size_t Capacity> class stack_allocator<void, Capacity> final : public allocator
{
  public:
    template <typename T> T *allocate()
    {
        return nallocate<T>(1);
    }
    template <typename T> T *nallocate(const std::size_t count)
    {
        KIT_ASSERT_ERROR(count > 0, "Cannot allocate zero elements");
        const std::size_t size = count * sizeof(T);
        KIT_ASSERT_ERROR(Capacity * sizeof(T) - m_memory.size() >= size,
                         "Out of memory! Requested {0} bytes, but only {1} bytes are available", size,
                         Capacity * sizeof(T) - m_memory.size());

        std::byte *ptr = (std::byte *)(m_memory.data() + m_memory.size());
        m_memory.resize(m_memory.size() + size);
        m_entries.push_back({ptr, size});
        return (T *)ptr;
    }

    template <typename T, class... Args> T *create(Args &&...args)
    {
        T *ptr = allocate<T>();
        allocator::construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }
    template <typename T, class... Args> T *ncreate(const std::size_t count, Args &&...args)
    {
        T *ptr = nallocate<T>(count);
        allocator::nconstruct(ptr, count, std::forward<Args>(args)...);
        return ptr;
    }

    void deallocate(void *ptr)
    {
        KIT_ASSERT_ERROR(!m_memory.empty(), "Cannot deallocate from an empty stack allocator");
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", ptr);
        KIT_ASSERT_ERROR(can_deallocate(ptr), "A stack allocator can only deallocate the last allocated object");
        m_memory.resize(m_memory.size() - m_entries.back());
        m_entries.pop_back();
    }

    template <typename T> void destroy(T *ptr)
    {
        allocator::deconstruct(ptr);
        deallocate(ptr);
    }
    template <typename T> void ndestroy(T *ptr, const std::size_t count)
    {
        allocator::ndeconstruct(ptr, count);
        deallocate(ptr);
    }

    bool owns(const void *ptr) const
    {
        return ptr >= m_memory.data() && ptr < m_memory.data() + m_memory.size();
    }
    bool can_deallocate(const void *ptr) const
    {
        return ptr == m_entries.back().ptr;
    }
    std::size_t remaining_size() const
    {
        return Capacity - m_memory.size();
    }

  private:
    struct entry
    {
        std::byte *ptr;
        std::size_t size;
    };
    dynarray<std::byte, Capacity> m_memory;
    dynarray<entry, Capacity> m_entries;
};

template <> class stack_allocator<void, 0> final : public allocator
{
  public:
    stack_allocator(const std::size_t stack_capacity) : m_stack_capacity(stack_capacity)
    {
    }

    template <typename T> T *allocate()
    {
        return nallocate<T>(1);
    }
    template <typename T> T *nallocate(const std::size_t count)
    {
        KIT_ASSERT_ERROR(count > 0, "Cannot allocate zero elements");
        const std::size_t size = count * sizeof(T);
        KIT_ASSERT_ERROR(m_stack_capacity >= size,
                         "Out of memory! Requested {0} bytes, but only {1} bytes are available per stack", size,
                         m_stack_capacity);

        if (m_stacks.empty() || size > m_stack_capacity - m_current_stack_size)
            return first_element_of_new_stack(count);
        return next_free_element_of_current_stack(count);
    }

    template <typename T, class... Args> T *create(Args &&...args)
    {
        T *ptr = allocate<T>();
        allocator::construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }
    template <typename T, class... Args> T *ncreate(const std::size_t count, Args &&...args)
    {
        T *ptr = nallocate<T>(count);
        allocator::nconstruct(ptr, count, std::forward<Args>(args)...);
        return ptr;
    }

    template <typename T> void deallocate(T *ptr)
    {
        KIT_ASSERT_ERROR(!m_stacks.empty(), "Cannot deallocate from an empty stack allocator");
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        KIT_ASSERT_ERROR(can_deallocate(ptr), "A stack allocator can only deallocate the last allocated object");
        if (m_current_stack_size == 0)
        {
            m_current_stack--;
            m_current_stack_size = m_entries.back().stack_size;
        }
        else
            m_current_stack_size -= m_entries.back().alloc_size;
        m_entries.pop_back();
    }

    bool owns(const void *ptr) const
    {
        for (std::byte *stack : m_stacks)
            if (ptr >= stack && ptr < stack + m_stack_capacity)
                return true;
        return false;
    }
    bool can_deallocate(const void *ptr) const
    {
        return ptr == m_entries.back().ptr;
    }

  private:
    template <typename T> T *first_element_of_new_stack(const std::size_t count)
    {
        const std::size_t size = count * sizeof(T);
        std::byte *stack = (std::byte *)std::malloc(size);

        m_current_stack = m_stacks.size();
        m_stacks.push_back(stack);
        m_current_stack_size = size;
        m_entries.push_back({stack, size, size});
        return (T *)stack;
    }

    template <typename T> T *next_free_element_of_current_stack(const std::size_t count)
    {
        std::byte *ptr = m_stacks[m_current_stack] + m_current_stack_size;

        const std::size_t size = count * sizeof(T);
        m_current_stack_size += size;
        m_entries.push_back({ptr, size, m_current_stack_size});
        return (T *)ptr;
    }

  private:
    struct entry
    {
        std::byte *ptr;
        std::size_t alloc_size;
        std::size_t stack_size;
    };

    std::vector<std::byte *> m_stacks;
    std::vector<entry> m_entries;
    std::size_t m_stack_capacity;
    std::size_t m_current_stack;
    std::size_t m_current_stack_size = 0;
};

} // namespace kit