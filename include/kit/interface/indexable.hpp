#ifndef KIT_INDEXABLE_HPP
#define KIT_INDEXABLE_HPP

#include <cstddef>

namespace kit
{
class indexable
{
  public:
    indexable() = default;
    explicit indexable(std::size_t index);

    std::size_t index() const;
    void index(std::size_t index);

  private:
    std::size_t m_index = 0;
};
} // namespace kit

#endif