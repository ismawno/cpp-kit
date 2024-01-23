#include "kit/memory/track_ptr.hpp"
#include <vector>

namespace kit
{
template <typename T, class... Args> using vector_ptr = track_ptr<std::vector<T, Args...>>;
template <typename T, class... Args> using const_vector_ptr = track_ptr<const std::vector<T, Args...>>;
} // namespace kit