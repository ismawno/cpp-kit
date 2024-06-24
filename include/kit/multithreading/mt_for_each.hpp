#pragma once

#include "kit/debug/log.hpp"
#include "kit/multithreading/thread_pool.hpp"
#include "kit/utility/type_constraints.hpp"
#include <thread>
#include <functional>

namespace kit::mt
{
template <typename T>
concept ValidContainer = RandomAccessContainer<T> && requires(T a) {
    {
        a.size()
    } -> std::convertible_to<std::size_t>;
};

template <std::size_t ThreadCount, ValidContainer C, typename F> struct type_helper
{
    using iterator_t = decltype(std::declval<C>().begin());
    static inline const auto worker = [](iterator_t it1, iterator_t it2, F fun, const std::size_t thread_idx) {
        for (auto it = it1; it != it2; ++it)
            fun(thread_idx, *it);
    };

    using worker_t = decltype(worker);
    static inline thread_pool<worker_t, iterator_t, iterator_t, F, std::size_t> pool{ThreadCount};
};

// consider having thread count as runtime parameter
template <std::size_t ThreadCount, ValidContainer C, typename F> void for_each(C &container, F fun)
{
    auto &pool = type_helper<ThreadCount, C, F>::pool;
    const std::size_t size = container.size();

    for (std::size_t i = 0; i < ThreadCount; i++)
    {
        const std::size_t start = i * size / ThreadCount;
        const std::size_t end = (i + 1) * size / ThreadCount;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        pool.submit(type_helper<ThreadCount, C, F>::worker, container.begin() + start, container.begin() + end, fun, i);
    }
    pool.await_pending();
}

template <std::size_t ThreadCount, ValidContainer C, typename F>
void for_each(C &container, F fun, const std::array<std::size_t, ThreadCount - 1> &splits)
{
    auto &pool = type_helper<ThreadCount, C, F>::pool;
    const std::size_t size = container.size();

    for (std::size_t i = 0; i < ThreadCount; i++)
    {
        const std::size_t start = i == 0 ? 0 : splits[i - 1];
        const std::size_t end = i == (ThreadCount - 1) ? size : splits[i];
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        pool.submit(type_helper<ThreadCount, C, F>::worker, container.begin() + start, container.begin() + end, fun, i);
    }
    pool.await_pending();
}

} // namespace kit::mt