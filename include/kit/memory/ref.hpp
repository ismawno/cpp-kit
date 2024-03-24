#pragma once

#include <memory>

namespace kit
{
template <typename T> using ref = std::shared_ptr<T>;

template <typename T, std::size_t BlockSize = 16 * 1024, std::size_t MaxChunkSize = 1024,
          std::uint32_t SupportedSizesIncrement = 32, class... Args>
inline ref<T> make_ref(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
} // namespace kit
