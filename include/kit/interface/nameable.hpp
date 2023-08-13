#ifndef KIT_NAMEABLE_HPP
#define KIT_NAMEABLE_HPP

namespace kit
{
class nameable
{
  public:
    explicit nameable(const char *name);

    const char *name;
};
} // namespace kit

#endif