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
concept Toggleable = std::is_base_of_v<kit::toggleable, T>;
} // namespace kit
