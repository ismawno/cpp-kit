#pragma once

#include "kit/debug/log.hpp"
#include "kit/memory/allocator/allocator.hpp"
#include <cstdlib>
#include <unordered_set>

namespace kit
{
template <typename T> class vanilla_allocator final : public continuous_allocator<T>
{
  public:
    ~vanilla_allocator()
    {
        for (T *ptr : m_allocated)
            continuous_allocator<T>::platform_aware_aligned_dealloc(ptr);
    }

    T *nallocate(const std::size_t count) override
    {
        constexpr std::size_t align = alignof(T);
        const std::size_t size = continuous_allocator<T>::aligned_size(count * sizeof(T), align);

        T *ptr = (T *)continuous_allocator<T>::platform_aware_aligned_alloc(size, align);
        m_allocated.insert(ptr);
        return ptr;
    }
    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        m_allocated.erase(ptr);
        continuous_allocator<T>::platform_aware_aligned_dealloc(ptr);
    }

    bool owns(const T *ptr) const override
    {
        return m_allocated.contains(ptr);
    }

  private:
    std::unordered_set<const T *> m_allocated;
};
} // namespace kit