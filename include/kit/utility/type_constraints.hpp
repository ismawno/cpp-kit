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

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <typename It, typename T>
concept Iterator = requires(It it, T t) {
    {
        *it
    } -> std::convertible_to<T>;
    {
        ++it
    } -> std::same_as<It>;
};
} // namespace kit