#ifndef KIT_SCOPE_HPP
#define KIT_SCOPE_HPP

#ifdef KIT_USE_CUSTOM_ALLOC
#include "kit/memory/block_allocator.hpp"
#endif
#include <memory>
#include <cstddef>
#include <cstdint>

namespace kit
{
#ifdef KIT_USE_CUSTOM_ALLOC
template <typename T, std::size_t BlockSize = 8 * 1024, std::size_t MaxChunkSize = 1024,
          std::uint32_t SupportedSizesIncrement = 32>
using scope = std::unique_ptr<T, block_deleter<T, BlockSize, MaxChunkSize, SupportedSizesIncrement>>;

template <typename T, std::size_t BlockSize = 8 * 1024, std::size_t MaxChunkSize = 1024,
          std::uint32_t SupportedSizesIncrement = 32, class... Args>
inline scope<T> make_scope(Args &&...args)
{
    if (!block_allocator<BlockSize, MaxChunkSize, SupportedSizesIncrement>::template can_allocate<T>())
        return scope<T>(new T(std::forward<Args>(args)...));

    T *buff = block_allocator<BlockSize, MaxChunkSize, SupportedSizesIncrement>::template allocate<T>();
    T *ptr = new (buff) T(std::forward<Args>(args)...);
    return scope<T>(ptr);
}

#else

template <typename T, std::size_t BlockSize = 8 * 1024, std::size_t MaxChunkSize = 1024,
          std::uint32_t SupportedSizesIncrement = 32>
using scope = std::unique_ptr<T>;

template <typename T, std::size_t BlockSize = 8 * 1024, std::size_t MaxChunkSize = 1024,
          std::uint32_t SupportedSizesIncrement = 32, class... Args>
inline scope<T> make_scope(Args &&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

#endif
} // namespace kit

#endif