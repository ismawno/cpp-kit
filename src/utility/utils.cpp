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
float cross2D(const glm::vec2 &v1, const glm::vec2 &v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}
} // namespace kit