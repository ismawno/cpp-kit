#pragma once

namespace kit
{
class toggleable
{
  public:
    explicit toggleable(bool enabled = true);

    bool enabled;

    void enable();
    void disable();
};

template <typename T>
concept Toggleable = requires(T a) {
    std::is_base_of_v<kit::toggleable, T>;
    {
        a.enabled
    } -> std::convertible_to<bool>;
    {
        a.enable()
    } -> std::convertible_to<void>;
    {
        a.disable()
    } -> std::convertible_to<void>;
};
} // namespace kit
