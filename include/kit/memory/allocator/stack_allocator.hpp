#pragma once

#include "kit/memory/allocator/allocator.hpp"
#include "kit/container/dynarray.hpp"
#include <vector>

namespace kit
{
template <typename T, std::size_t Capacity = SIZE_MAX> class stack_allocator final : public continuous_allocator<T>
{
  public:
    T *nallocate(const std::size_t count) override
    {
        KIT_ASSERT_ERROR(count > 0, "Cannot allocate zero elements");
        KIT_ASSERT_ERROR(Capacity - m_memory.size() >= count,
                         "Out of memory! Requested {0} objects, but only {1} slots are available", count,
                         Capacity - m_memory.size());
        T *ptr = m_memory.data() + m_memory.size();
        m_memory.resize(m_memory.size() + count);
        m_allocation_sizes.push_back(count);
        return ptr;
    }
    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(!m_memory.empty(), "Cannot deallocate from an empty stack allocator");
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        KIT_ASSERT_ERROR(can_deallocate(ptr), "A stack allocator can only deallocate the last allocated object");
        m_memory.resize(m_memory.size() - m_allocation_sizes.back());
        m_allocation_sizes.pop_back();
    }

    bool owns(const T *ptr) const override
    {
        return ptr >= m_memory.data() && ptr < m_memory.data() + m_memory.size();
    }
    bool can_deallocate(const T *ptr) const
    {
        return ptr == m_memory.data() + m_memory.size() - m_allocation_sizes.back();
    }
    std::size_t remaining_slots() const
    {
        return Capacity - m_memory.size();
    }

  private:
    dynarray<T, Capacity> m_memory;
    dynarray<std::size_t, Capacity> m_allocation_sizes;
};

template <typename T> class stack_allocator<T, SIZE_MAX> final : public continuous_allocator<T>
{
  public:
    stack_allocator(const std::size_t stack_capacity) : m_stack_capacity(stack_capacity)
    {
    }

    T *nallocate(const std::size_t count) override
    {
        KIT_ASSERT_ERROR(count > 0, "Cannot allocate zero elements");
        KIT_ASSERT_ERROR(m_stack_capacity >= count,
                         "Out of memory! Requested {0} objects, but only {1} slots are available", count,
                         m_stack_capacity);

        if (m_stacks.empty() || count <= m_stack_capacity - m_current_stack_size)
            return first_element_of_new_stack(count);
        return next_free_element_of_current_stack(count);
    }
    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(!m_stacks.empty(), "Cannot deallocate from an empty stack allocator");
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        KIT_ASSERT_ERROR(can_deallocate(ptr), "A stack allocator can only deallocate the last allocated object");
        if (m_current_stack_size == 0)
        {
            m_current_stack--;
            m_current_stack_size = m_allocation_sizes.back().stack_size;
        }
        else
            m_current_stack_size -= m_allocation_sizes.back().alloc_size;
        m_allocation_sizes.pop_back();
    }

    bool owns(const T *ptr) const override
    {
        for (T *stack : m_stacks)
            if (ptr >= stack && ptr < stack + m_stack_capacity)
                return true;
        return false;
    }
    bool can_deallocate(const T *ptr) const
    {
        const std::size_t offset = m_allocation_sizes.back().stack_size - m_allocation_sizes.back().alloc_size;
        return ptr == m_stacks[m_current_stack] + offset;
    }

  private:
    T *first_element_of_new_stack(const std::size_t count)
    {
        constexpr std::size_t align = alignof(T);
        const std::size_t size = continuous_allocator<T>::aligned_size(count * sizeof(T), align);
        T *ptr = (T *)continuous_allocator<T>::platform_aware_aligned_alloc(size, align);

        m_current_stack = m_stacks.size();
        m_stacks.push_back(ptr);
        m_current_stack_size = count;
        m_allocation_sizes.push_back({count, count});
        return ptr;
    }

    T *next_free_element_of_current_stack(const std::size_t count)
    {
        T *ptr = m_stacks[m_current_stack] + m_current_stack_size;
        m_current_stack_size += count;
        m_allocation_sizes.push_back({count, m_current_stack_size});
        return ptr;
    }

  private:
    struct alloc_entry
    {
        std::size_t alloc_size;
        std::size_t stack_size;
    };

    std::vector<T *> m_stacks;
    std::vector<alloc_entry> m_allocation_sizes;
    std::size_t m_stack_capacity;
    std::size_t m_current_stack;
    std::size_t m_current_stack_size;
};

} // namespace kit