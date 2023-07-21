#ifndef KIT_TOGGLEABLE_HPP
#define KIT_TOGGLEABLE_HPP

namespace kit
{
class toggleable
{
  public:
    toggleable(bool enabled = true);

    bool enabled() const;
    void enabled(bool enabled);

    void enable();
    void disable();

  private:
    bool m_enabled;
};
} // namespace kit

#endif