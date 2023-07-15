#ifndef KIT_SCOPE_HPP
#define KIT_SCOPE_HPP

#ifdef KIT_USE_CUSTOM_ALLOC
#include "kit/memory/block_allocator.hpp"
#endif
#include <memory>

namespace kit
{
#ifdef KIT_USE_CUSTOM_ALLOC
template <typename T> using scope = std::unique_ptr<T, block_deleter<T>>;

template <typename T, class... Args> inline scope<T> make_scope(Args &&...args)
{
    block_allocator<T> alloc; // I dont think static is even worth it
    T *buff = alloc.allocate_raw(sizeof(T));
    if (!buff)
        return scope<T>(new T(std::forward<Args>(args)...));

    T *p = new (buff) T(std::forward<Args>(args)...);
    KIT_ASSERT_WARN((std::uint64_t)p % alignof(T) == 0, "Block allocated pointer {0} is not aligned! Alignment: {1}",
                    (void *)p, alignof(T))
    return scope<T>(p);
}

#else

template <typename T> using scope = std::unique_ptr<T>;
template <typename T, class... Args> inline scope<T> make_scope(Args &&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

#endif
} // namespace kit

#endif