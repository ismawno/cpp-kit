#ifndef KIT_TOGGLEABLE_HPP
#define KIT_TOGGLEABLE_HPP

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

#endif