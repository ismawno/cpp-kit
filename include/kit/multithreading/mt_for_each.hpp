#pragma once

#include "kit/debug/log.hpp"
#include "kit/multithreading/thread_pool.hpp"
#include "kit/utility/type_constraints.hpp"
#include <thread>
#include <functional>

namespace kit::mt
{
template <RandomAccessContainer C> using _feach_iterator = decltype(std::declval<C>().begin());
template <RandomAccessContainer C> using _feach_value_type = decltype(*std::declval<C>().begin());

template <typename T> using _feach_fun = std::function<void(std::size_t, T &)>;
template <RandomAccessContainer C>
using feach_thread_pool =
    thread_pool<_feach_iterator<C>, _feach_iterator<C>, _feach_fun<_feach_value_type<C>>, std::size_t>;

template <typename T, typename It>
void _for_each_worker(It it1, It it2, _feach_fun<T> fun, const std::size_t thread_idx)
{
    for (auto it = it1; it != it2; ++it)
        fun(thread_idx, *it);
}

template <RandomAccessContainer C>
void for_each(C &container, feach_thread_pool<C> &pool, _feach_fun<_feach_value_type<C>> fun)
{
    const std::size_t size = container.size();
    for (std::size_t i = 0; i < pool.size(); i++)
    {
        const std::size_t start = i * size / pool.size();
        const std::size_t end = (i + 1) * size / pool.size();
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        pool.submit(_for_each_worker<_feach_value_type<C>, decltype(container.begin())>, container.begin() + start,
                    container.begin() + end, fun, i);
    }
    pool.await_pending();
}
} // namespace kit::mt