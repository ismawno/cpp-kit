#pragma once

#include "kit/debug/log.hpp"
#include "kit/memory/allocator/allocator.hpp"
#include <cstdlib>
#include <unordered_set>

namespace kit
{
template <typename T> class vanilla_allocator final : continuous_allocator<T>
{
  public:
    ~vanilla_allocator()
    {
        for (T *ptr : m_allocated)
            std::free(ptr);
    }

    T *nallocate(const std::size_t count) override
    {
        T *ptr = (T *)std::malloc(count * sizeof(T));
        KIT_ASSERT_ERROR(ptr, "Failed to allocate {0} bytes", count * sizeof(T));
        m_allocated.insert(ptr);
        return ptr;
    }
    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);
        m_allocated.erase(ptr);
        std::free(ptr);
    }

    bool owns(const T *ptr) const override
    {
        return m_allocated.contains(ptr);
    }

  private:
    std::unordered_set<const T *> m_allocated;
};
} // namespace kit