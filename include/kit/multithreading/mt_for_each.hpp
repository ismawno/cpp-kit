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

template <std::size_t ThreadCount, ValidContainer C, typename F> void for_each(C &container, F fun)
{
    using It = decltype(std::declval<C>().begin());

    const auto worker = [](It it1, It it2, F fun, const std::size_t thread_idx) {
        for (auto it = it1; it != it2; ++it)
            fun(thread_idx, *it);
    };

    static thread_pool<decltype(worker), It, It, F, std::size_t> pool{ThreadCount};

    const std::size_t size = container.size();
    for (std::size_t i = 0; i < pool.size(); i++)
    {
        const std::size_t start = i * size / pool.size();
        const std::size_t end = (i + 1) * size / pool.size();
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        pool.submit(worker, container.begin() + start, container.begin() + end, fun, i);
    }
    pool.await_pending();
}
} // namespace kit::mt