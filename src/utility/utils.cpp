#include "kit/internal/pch.hpp"
#include "kit/utility/utils.hpp"

namespace kit
{
bool approximately(float a, float b)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    return std::abs(a - b) <= epsilon;
}
bool approaches_zero(float x)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    return std::abs(x) <= epsilon;
}
} // namespace kit