#pragma once

#include <concepts>

namespace kit
{
template <typename Derived, typename Base>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

template <typename T>
concept Hashable = requires(T a) {
    {
        std::hash<T>()(a)
    } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T, class... Args>
concept NoCopyCtorOverride =
    (!std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Args>> && ...) || sizeof...(Args) != 1;
} // namespace kit