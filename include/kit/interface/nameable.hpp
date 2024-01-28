#pragma once

namespace kit
{
class nameable
{
  public:
    explicit nameable(const char *name);

    const char *name;
};

template <typename T>
concept Nameable = std::is_base_of_v<kit::nameable, T>;

} // namespace kit
