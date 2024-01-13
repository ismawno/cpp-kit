#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>

#include "kit/utility/type_constraints.hpp"

namespace kit
{
bool approximately(float a, float b);
bool approaches_zero(float x);
float cross2D(const glm::vec2 &v1, const glm::vec2 &v2);

template <typename Derived, template <typename...> typename SmartPtr, typename Base>
    requires DerivedFrom<Derived, Base>
const Derived *const_get_casted_raw_ptr(const SmartPtr<Base> &ptr)
{
    if constexpr (std::is_same_v<Base, Derived>)
        return ptr.get();
    else
        return dynamic_cast<const Derived *>(ptr.get());
}

template <typename Derived, template <typename...> typename SmartPtr, typename Base>
    requires DerivedFrom<Derived, Base>
Derived *get_casted_raw_ptr(const SmartPtr<Base> &ptr)
{
    if constexpr (std::is_same_v<Base, Derived>)
        return ptr.get();
    else
        return dynamic_cast<Derived *>(ptr.get());
}
} // namespace kit
