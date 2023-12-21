#pragma once

namespace kit
{
template <typename T, typename F, T... Seq>
constexpr void static_for_each(std::integer_sequence<T, Seq...>, const F &fun)
{
    (fun(std::integral_constant<T, Seq>{}), ...);
}

template <auto n, typename F> constexpr void static_for_each(const F &fun)
{
    static_for_each(std::make_integer_sequence<decltype(n), n>{}, fun);
}
} // namespace kit