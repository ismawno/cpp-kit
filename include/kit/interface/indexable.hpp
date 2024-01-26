#pragma once

#include <cstddef>

namespace kit
{
class indexable
{
  public:
    explicit indexable(std::size_t index = 0);

    std::size_t index;
};

template <typename T>
concept Indexable = requires(T a) {
    std::is_base_of_v<kit::indexable, T>;
    {
        a.index
    } -> std::convertible_to<std::size_t>;
};
} // namespace kit
