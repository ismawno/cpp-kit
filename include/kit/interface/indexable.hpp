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
} // namespace kit
