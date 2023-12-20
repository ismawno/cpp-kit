#pragma once

namespace kit
{
class nameable
{
  public:
    explicit nameable(const char *name);

    const char *name;
};
} // namespace kit
