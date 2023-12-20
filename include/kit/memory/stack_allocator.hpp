#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include "kit/debug/log.hpp"

namespace kit
{
template <typename T, std::size_t Capacity = 1024, std::uint32_t ID = 0>
class stack_allocator : public std::allocator<T>
{
  private:
    using base = std::allocator<T>;
    using ptr = typename std::allocator_traits<base>::pointer;
    using size = typename std::allocator_traits<base>::size_type;

    struct aligned_type
    {
        alignas(T) std::byte pointer[sizeof(T)];
    };
    struct entry
    {
        ptr alloc_ptr;
        size alloc_size;
        bool stack_allocated;
    };

    inline static std::array<aligned_type, Capacity> s_buffer{};
    inline static std::size_t s_index = 0;
    inline static entry s_last_entry;

  public:
    stack_allocator() noexcept = default;
    template <typename U> stack_allocator(const stack_allocator<U> &other)
    {
    }

    template <typename U> struct rebind
    {
        using other = stack_allocator<U>;
    };

    ptr allocate(size n)
    {
        KIT_ASSERT_CRITICAL(n > 0, "Attempting to allocate a non-positive amount of memory: {0}", n)
        KIT_ASSERT_DEBUG(s_index + n <= Capacity,
                         "Stack allocator capacity of {0} reached when allocating {1} elements of size {2}. Falling "
                         "back to default allocator",
                         Capacity, n, sizeof(T))
        if (s_index + n <= Capacity)
        {
            const ptr p = base::allocate(n);
            s_last_entry = {p, n, false};
            return p;
        }

        const ptr p = (ptr)(s_buffer[s_index].pointer);
        s_index += n;
        s_last_entry = {p, n, true};
        return p;
    }

    void deallocate(ptr p, size n)
    {
        KIT_ASSERT_CRITICAL(n > 0, "Attempting to deallocate a non-positive amount of memory: {0}", n)
        KIT_ASSERT_ERROR(p, "Attempting to deallocate null pointer in stack allocator")
        KIT_ASSERT_CRITICAL(p == s_last_entry.alloc_ptr,
                            "Attempting to deallocate {0} elements of {1} bytes from an adress that is either not on "
                            "top of the allocator or does not belong to it",
                            n, sizeof(T))
        KIT_ASSERT_CRITICAL(n == s_last_entry.alloc_size,
                            "Last allocation entry size mismatch! Attempting to stack deallocate with a different "
                            "size: current: {0}, last: {1}",
                            n, s_index)
        if (s_last_entry.stack_allocated)
            s_index -= n;
        else
            base::deallocate(p, n);
    }
};

template <typename T, std::size_t Capacity, std::uint32_t ID = 0> struct stack_deleter
{
    constexpr stack_deleter() noexcept = default;

    void operator()(T *p)
    {
        stack_allocator<T, Capacity, ID> alloc;
        alloc.deallocate(p, 1);
    }
};
} // namespace kit
