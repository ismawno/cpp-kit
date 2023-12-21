#pragma once

#include <tuple>
#include <functional>

namespace kit
{
enum class hash
{
    asymmetric,
    symmetric
};
template <hash HashType, class... Hashable> struct multi_hash
{
    multi_hash(Hashable &&...hashable) : elms(hashable...)
    {
    }

    std::tuple<Hashable...> elms;

    std::size_t operator()() const
    {
        return (*this)(std::make_integer_sequence<int, sizeof...(Hashable)>{});
    }

    bool operator==(const multi_hash &other) const
    {
        return elms == other.elms;
    }

  private:
    inline static constexpr std::size_t SEED = 0x517cc1b7;
    template <int... ints> std::size_t operator()(std::integer_sequence<int, ints...> int_seq) const
    {
        std::size_t seed = SEED;
        hash_combine(seed, std::get<ints>(elms)...);
        return seed;
    }

    template <typename T, class... Rest> static void hash_combine(std::size_t &seed, const T &hashable, Rest &&...rest)
    {
        const std::hash<T> hasher;
        if constexpr (HashType == hash::asymmetric)
            seed ^= hasher(hashable) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        else
            seed ^= hasher(hashable) + 0x9e3779b9 + (SEED << 6) + (SEED >> 2);
        if constexpr (sizeof...(rest) != 0)
            hash_combine(seed, rest...);
    }
};

template <class... Hashable> using asymm_multi_hash = multi_hash<hash::asymmetric, Hashable...>;
template <class... Hashable> using symm_multi_hash = multi_hash<hash::symmetric, Hashable...>;
} // namespace kit

namespace std
{
template <kit::hash HashType, class... Hashable> struct hash<kit::multi_hash<HashType, Hashable...>>
{
    size_t operator()(const kit::multi_hash<HashType, Hashable...> &mhash) const
    {
        return mhash();
    }
};
} // namespace std