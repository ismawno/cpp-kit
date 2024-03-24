#pragma once

#include <memory>

namespace kit
{
template <typename T> using scope = std::unique_ptr<T>;

template <typename T, class... Args> inline scope<T> make_scope(Args &&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}
} // namespace kit
