#pragma once

#include <concepts>
#include <iterator>

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
concept RandomAccessContainer = requires(T a) {
    {
        a.begin()
    } -> std::random_access_iterator;
    {
        a.end()
    } -> std::random_access_iterator;

    {
        a.size()
    } -> std::convertible_to<std::size_t>;
    {
        a.empty()
    } -> std::convertible_to<bool>;
};

template <typename T, class... Args>
concept NoCopyCtorOverride =
    (!std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Args>> && ...) || sizeof...(Args) != 1;

template <typename F, class... Args>
concept Callable = std::invocable<F, Args...>;

template <typename F, typename R, class... Args>
concept RetCallable = std::invocable<F, Args...> && std::convertible_to<std::invoke_result_t<F, Args...>, R>;

template <typename F, class... Args>
concept VoidCallable = std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, void>;
} // namespace kit