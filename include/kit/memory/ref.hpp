#pragma once

#include <memory>

namespace kit
{
template <typename T> using ref = std::shared_ptr<T>;

template <typename T, class... Args> inline ref<T> make_ref(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
} // namespace kit
