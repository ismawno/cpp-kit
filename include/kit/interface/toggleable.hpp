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
} // namespace kit
