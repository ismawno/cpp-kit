#ifndef KIT_REF_HPP
#define KIT_REF_HPP

#ifdef KIT_USE_CUSTOM_ALLOC
#include "kit/memory/block_allocator.hpp"
#endif
#include <memory>

namespace kit
{
template <typename T> using ref = std::shared_ptr<T>;

#ifdef KIT_USE_CUSTOM_ALLOC

template <typename T, class... Args> inline ref<T> make_ref(Args &&...args)
{
    static kit::block_allocator<T> alloc;
    T *buff = alloc.allocate_raw(sizeof(T));
    if (!buff)
        return std::make_shared<T>(std::forward<Args>(args)...);

    T *p = new (buff) T(std::forward<Args>(args)...);
    DBG_ASSERT_WARN((std::uint64_t)p % alignof(T) == 0, "Block allocated pointer {0} is not aligned! Alignment: {1}",
                    (void *)p, alignof(T))
    return ref<T>(p, kit::block_deleter<T>());

#else

template <typename T, class... Args> inline ref<T> make_ref(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#endif
}
} // namespace kit

#endif