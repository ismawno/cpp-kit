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

template <typename T, typename Tuple> struct tuple_has_type;

template <typename T, typename Head, typename... Tail>
struct tuple_has_type<T, std::tuple<Head, Tail...>>
    : std::disjunction<std::is_same<T, Head>, tuple_has_type<T, std::tuple<Tail...>>>
{
};

template <typename T> struct tuple_has_type<T, std::tuple<>> : std::false_type
{
};

template <typename T, typename Tuple> inline constexpr bool tuple_has_type_v = tuple_has_type<T, Tuple>::value;

template <typename Derived, template <typename...> typename SmartPtr, typename Base>
    requires DerivedFrom<Derived, Base>
Derived *get_casted_raw_ptr(const SmartPtr<Base> &ptr)
{
    if constexpr (std::is_same_v<Base, std::remove_const_t<Derived>>)
        return ptr.get();
    else
        return dynamic_cast<Derived *>(ptr.get());
}

template <typename Derived, template <typename...> typename SmartPtr, typename Base, class... TupleArgs>
    requires DerivedFrom<Derived, Base>
Derived *get_casted_raw_ptr(const SmartPtr<Base> &ptr, const std::tuple<TupleArgs *...> &known)
{
    if constexpr (std::is_same_v<Base, std::remove_const_t<Derived>>)
        return ptr.get();
    else if constexpr (tuple_has_type_v<Derived *, std::tuple<TupleArgs *...>>)
        return std::get<Derived *>(known);
    else
        return dynamic_cast<Derived *>(ptr.get());
}
} // namespace kit
