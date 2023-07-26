#ifndef KIT_NAMEABLE_HPP
#define KIT_NAMEABLE_HPP

namespace kit
{
class nameable
{
  public:
    explicit nameable(const char *name);

    const char *name() const;
    void name(const char *name);

  private:
    const char *m_name;
};
} // namespace kit

#endif