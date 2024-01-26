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
concept Nameable = requires(T a) {
    std::is_base_of_v<kit::nameable, T>;
    {
        a.name
    } -> std::convertible_to<const char *>;
};
} // namespace kit
