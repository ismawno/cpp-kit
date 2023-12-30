#pragma once

#include "kit/utility/static_for.hpp"
#include <tuple>
#include <functional>

namespace kit
{
enum class hash
{
    non_commutative,
    commutative
};
template <hash HashType, class... Hashable> struct hashable_tuple
{
    hashable_tuple() = default;

    template <class... TupleArgs> hashable_tuple(TupleArgs &&...args) : elms(std::forward<TupleArgs>(args)...)
    {
    }

    std::tuple<Hashable...> elms;

    std::size_t operator()() const
    {
        return (*this)(std::make_integer_sequence<int, sizeof...(Hashable)>{});
    }

    template <class... OtherHashable> bool operator==(const hashable_tuple<HashType, OtherHashable...> &other) const
    {
        if constexpr (HashType == hash::non_commutative)
            return elms == other.elms;
        else if constexpr (sizeof...(Hashable) != sizeof...(OtherHashable))
            return false;
        else
        {
            bool are_equal1 = true;
            bool are_equal2 = true;
            kit::static_for_each<sizeof...(Hashable)>([&are_equal1, &are_equal2, &other, this](auto i) {
                bool any_equal1 = false;
                bool any_equal2 = false;
                kit::static_for_each<sizeof...(Hashable)>([&i, &any_equal1, &any_equal2, &other, this](auto j) {
                    any_equal1 |= compare(std::get<i>(elms), std::get<j>(other.elms));
                    any_equal2 |= compare(std::get<i>(other.elms), std::get<j>(elms));
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
        if constexpr (HashType == hash::non_commutative)
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

template <class... Hashable> using non_commutative_tuple = hashable_tuple<hash::non_commutative, Hashable...>;
template <class... Hashable> using commutative_tuple = hashable_tuple<hash::commutative, Hashable...>;
} // namespace kit

template <kit::hash HashType, class... Hashable> struct std::hash<kit::hashable_tuple<HashType, Hashable...>>
{
    std::size_t operator()(const kit::hashable_tuple<HashType, Hashable...> &mhash) const
    {
        return mhash();
    }
};
