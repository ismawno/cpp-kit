#ifndef KIT_NON_COPYABLE_HPP
#define KIT_NON_COPYABLE_HPP

namespace kit
{
class non_copyable
{
    non_copyable(const non_copyable &) = delete;
    non_copyable &operator=(const non_copyable &) = delete;

  protected:
    non_copyable() = default;
};
} // namespace kit
#endif