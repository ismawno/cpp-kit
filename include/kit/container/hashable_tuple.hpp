#pragma once

#include "kit/utility/static_for_each.hpp"
#include "kit/utility/type_constraints.hpp"
#include <tuple>
#include <functional>

namespace kit
{
enum class hash_property
{
    non_commutative,
    commutative
};

template <hash_property Property, Hashable... HT> struct hashable_tuple
{
    hashable_tuple() = default;

    template <class... TupleArgs>
        requires NoCopyCtorOverride<hashable_tuple, TupleArgs...>
    hashable_tuple(TupleArgs &&...args) : elms(std::forward<TupleArgs>(args)...)
    {
    }

    std::tuple<HT...> elms;

    std::size_t operator()() const
    {
        return (*this)(std::make_integer_sequence<int, sizeof...(HT)>{});
    }

    template <class... OtherHashable> bool operator==(const hashable_tuple<Property, OtherHashable...> &other) const
    {
        if constexpr (Property == hash_property::non_commutative)
            return elms == other.elms;
        else if constexpr (sizeof...(HT) != sizeof...(OtherHashable))
            return false;
        else
        {
            bool are_equal1 = true;
            bool are_equal2 = true;
            kit::static_for_each<sizeof...(HT)>([&are_equal1, &are_equal2, &other, this](auto i) {
                bool any_equal1 = false;
                bool any_equal2 = false;
                kit::static_for_each<sizeof...(HT)>([&i, &any_equal1, &any_equal2, &other, this](auto j) {
                    any_equal1 |= compare(std::get<i.value>(elms), std::get<j.value>(other.elms));
                    any_equal2 |= compare(std::get<i.value>(other.elms), std::get<j.value>(elms));
                });
                are_equal1 &= any_equal1;
                are_equal2 &= any_equal2;
            });
            return are_equal1 && are_equal2;
        }
    }

  private:
    inline static constexpr std::size_t SEED = 0x517cc1b7;
    template <int... Ints> std::size_t operator()(std::integer_sequence<int, Ints...> int_seq) const
    {
        std::size_t seed = SEED;
        hash_combine(seed, std::get<Ints>(elms)...);
        return seed;
    }

    template <typename T, class... Rest> static void hash_combine(std::size_t &seed, const T &hashable, Rest &&...rest)
    {
        const std::hash<T> hasher;
        if constexpr (Property == hash_property::non_commutative)
            seed ^= hasher(hashable) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        else
            seed ^= hasher(hashable) + 0x9e3779b9 + (SEED << 6) + (SEED >> 2);
        if constexpr (sizeof...(rest) != 0)
            hash_combine(seed, std::forward<Rest>(rest)...);
    }

    template <typename T, typename U> static bool compare(const T &t, const U &u)
    {
        if constexpr (!std::is_same_v<T, U>)
            return false;
        else
            return t == u;
    }
};

template <class... HT> using non_commutative_tuple = hashable_tuple<hash_property::non_commutative, HT...>;
template <class... HT> using commutative_tuple = hashable_tuple<hash_property::commutative, HT...>;
} // namespace kit

template <kit::hash_property Property, class... HT> struct std::hash<kit::hashable_tuple<Property, HT...>>
{
    std::size_t operator()(const kit::hashable_tuple<Property, HT...> &mhash) const
    {
        return mhash();
    }
};
