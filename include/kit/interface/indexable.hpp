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
concept Indexable = std::is_base_of_v<kit::indexable, T>;
} // namespace kit
