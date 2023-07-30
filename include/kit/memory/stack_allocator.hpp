#ifndef STACK_ALLOCATOR_HPP
#define STACK_ALLOCATOR_HPP

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

    inline static std::array<aligned_type, Capacity> s_buffer{};
    inline static std::size_t s_index = 0;

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
        KIT_ASSERT_CRITICAL(s_index + n <= Capacity,
                            "Stack allocator capacity of {0} reached when allocating {1} elements of size {2}",
                            Capacity, n, sizeof(T))
        const ptr p = (ptr)(s_buffer[s_index].pointer);
        s_index += n;
        return p;
    }

    void deallocate(ptr p, size n)
    {
        KIT_ASSERT_CRITICAL(n > 0, "Attempting to deallocate a non-positive amount of memory: {0}", n)
        KIT_ASSERT_ERROR(p, "Attempting to deallocate null pointer in stack allocator")
        KIT_ASSERT_CRITICAL(
            (s_index - n) >= 0,
            "Attempting to deallocate {0} elements, but there are only {1} elements remaining in stack allocator", n,
            s_index)
        KIT_ASSERT_CRITICAL((ptr)(s_buffer[s_index - n].pointer) == p,
                            "Attempting to deallocate {0} elements of {1} bytes disorderly from stack allocator", n,
                            sizeof(T))

        s_index -= n;
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

#endif