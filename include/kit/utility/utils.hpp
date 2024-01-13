#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>

namespace kit
{
bool approximately(float a, float b);
bool approaches_zero(float x);
float cross2D(const glm::vec2 &v1, const glm::vec2 &v2);

template <typename Base, typename Derived>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

template <typename Derived, template <typename...> typename SmartPtr, typename Base>
    requires DerivedFrom<Base, Derived>
const Derived *const_get_casted_raw_ptr(const SmartPtr<Base> &ptr)
{
    if constexpr (std::is_same_v<Base, Derived>)
        return ptr.get();
    else
        return dynamic_cast<const Derived *>(ptr.get());
}

template <typename Derived, template <typename...> typename SmartPtr, typename Base>
    requires DerivedFrom<Base, Derived>
Derived *get_casted_raw_ptr(const SmartPtr<Base> &ptr)
{
    if constexpr (std::is_same_v<Base, Derived>)
        return ptr.get();
    else
        return dynamic_cast<Derived *>(ptr.get());
}
} // namespace kit
