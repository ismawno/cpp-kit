#pragma once

#include "kit/debug/log.hpp"
#include "kit/interface/non_copyable.hpp"
#include <cstdlib>

namespace kit
{
class allocator : non_copyable
{
  public:
    virtual ~allocator() = default;

    template <typename T, class... Args> static void construct(T *ptr, Args &&...args)
    {
        new (ptr) T(std::forward<Args>(args)...);
    }
    template <typename T> static void deconstruct(T *ptr)
    {
        KIT_ASSERT_ERROR(ptr, "Cannot deconstruct a null pointer");
        ptr->~T();
    }

    template <typename T, class... Args> static void nconstruct(T *ptr, const std::size_t count, Args &&...args)
    {
        for (std::size_t i = 0; i < count; ++i)
            construct(ptr + i, std::forward<Args>(args)...);
    }
    template <typename T> static void ndeconstruct(T *ptr, const std::size_t count)
    {
        for (std::size_t i = 0; i < count; ++i)
            deconstruct(ptr + i);
    }

  protected:
    static void *platform_aware_aligned_alloc(const std::size_t size, const std::size_t align)
    {
#ifdef _MSC_VER
        return _aligned_malloc(size, align);
#else
        return std::aligned_alloc(align, size);
#endif
    }

    static void platform_aware_aligned_dealloc(void *ptr)
    {
#ifdef _MSC_VER
        _aligned_free(ptr);
#else
        std::free(ptr);
#endif
    }

    static inline constexpr std::size_t aligned_size(const std::size_t size, const std::size_t align)
    {
        const std::size_t remainder = size % align;
        if (remainder == 0)
            return size;
        return size + align - remainder;
    }
};

template <typename T> class discrete_allocator : public allocator
{
  public:
    virtual T *allocate() = 0;
    virtual void deallocate(T *ptr) = 0;
    virtual bool owns(const T *ptr) const = 0;

    template <class... Args> T *create(Args &&...args)
    {
        T *ptr = allocate();
        construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }
    void destroy(T *ptr)
    {
        deconstruct(ptr);
        deallocate(ptr);
    }
};

template <typename T> class continuous_allocator : public discrete_allocator<T>
{
  public:
    virtual T *nallocate(std::size_t count) = 0;

    virtual T *allocate() override
    {
        return nallocate(1);
    }

    template <class... Args> T *ncreate(const std::size_t count, Args &&...args)
    {
        T *ptr = nallocate(count);
        nconstruct(ptr, count, std::forward<Args>(args)...);
        return ptr;
    }
    void ndestroy(T *ptr, const std::size_t count)
    {
        ndeconstruct(ptr, count);
        deallocate(ptr);
    }
};

} // namespace kit