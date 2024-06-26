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

template <typename C, typename F, class... Args> struct type_helper
{
    using iterator_t = decltype(std::declval<C>().begin());
    static inline const auto worker = [](iterator_t it1, iterator_t it2, F fun, const std::size_t workload_index,
                                         Args &&...args) {
        for (auto it = it1; it != it2; ++it)
            fun(workload_index, *it, std::forward<Args>(args)...);
    };
};

template <ValidContainer C, typename F, class... Args>
void for_each(thread_pool &pool, C &container, F fun, const std::size_t workloads, Args &&...args)
{
    KIT_ASSERT_ERROR(workloads != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(workloads > 1, "Parallel for each purpose is defeated with only a single workload")

    const std::size_t size = container.size();
    for (std::size_t i = 0; i < workloads; i++)
    {
        const std::size_t start = i * size / workloads;
        const std::size_t end = (i + 1) * size / workloads;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            pool.submit(type_helper<C, F, Args...>::worker, container.begin() + start, container.begin() + end, fun, i,
                        std::forward<Args>(args)...);
    }
    pool.await_pending();
}

template <ValidContainer C, typename F, class... Args>
void for_each(thread_pool &pool, C &container, F fun, const std::vector<std::size_t> &splits, Args &&...args)
{
    KIT_ASSERT_ERROR(splits.size() != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(splits.size() > 1, "Parallel for each purpose is defeated with only a single workload")

    const std::size_t size = container.size();
    for (std::size_t i = 0; i < splits.size(); i++)
    {
        const std::size_t start = i == 0 ? 0 : splits[i - 1];
        const std::size_t end = i == (splits.size() - 1) ? size : splits[i];
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            pool.submit(type_helper<C, F, Args...>::worker, container.begin() + start, container.begin() + end, fun, i,
                        std::forward<Args>(args)...);
    }
    pool.await_pending();
}

template <ValidContainer C, typename F, class... Args>
void for_each_async(C &container, F fun, const std::size_t workloads, Args &&...args)
{
    KIT_ASSERT_ERROR(workloads != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(workloads > 1, "Parallel for each purpose is defeated with only a single workload")

    const std::size_t size = container.size();
    std::vector<std::future<void>> futures;
    for (std::size_t i = 0; i < workloads; i++)
    {
        const std::size_t start = i * size / workloads;
        const std::size_t end = (i + 1) * size / workloads;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            futures.emplace_back(std::async(std::launch::async, type_helper<C, F, Args...>::worker,
                                            container.begin() + start, container.begin() + end, fun, i,
                                            std::forward<Args>(args)...));
    }

    for (auto &future : futures)
        future.get();
}

template <ValidContainer C, typename F, class... Args>
void for_each_async(C &container, F fun, const std::vector<std::size_t> &splits, Args &&...args)
{
    KIT_ASSERT_ERROR(splits.size() != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(splits.size() > 1, "Parallel for each purpose is defeated with only a single workload")

    const std::size_t size = container.size();
    std::vector<std::future<void>> futures;
    for (std::size_t i = 0; i < splits.size(); i++)
    {
        const std::size_t start = i == 0 ? 0 : splits[i - 1];
        const std::size_t end = i == (splits.size() - 1) ? size : splits[i];
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            futures.emplace_back(std::async(std::launch::async, type_helper<C, F, Args...>::worker,
                                            container.begin() + start, container.begin() + end, fun, i,
                                            std::forward<Args>(args)...));
    }

    for (auto &future : futures)
        future.get();
}

} // namespace kit::mt