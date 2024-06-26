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
    static inline const auto worker = [](iterator_t it1, iterator_t it2, F fun, const std::size_t submission_index) {
        for (auto it = it1; it != it2; ++it)
            fun(submission_index, *it);
    };

    using worker_t = decltype(worker);
    static inline thread_pool<worker_t, iterator_t, iterator_t, F, std::size_t> pool{ThreadCount};
};

// consider having thread count as runtime parameter
template <std::size_t ThreadCount, ValidContainer C, typename F>
void for_each(C &container, F fun, const std::size_t submissions = ThreadCount)
{
    KIT_ASSERT_ERROR(submissions != 0, "Submission count must be greater than 0")
    KIT_ASSERT_WARN(submissions > 1, "Parallel for each purpose is defeated with only a single submission")

    auto &pool = type_helper<ThreadCount, C, F>::pool;
    const std::size_t size = container.size();

    for (std::size_t i = 0; i < submissions; i++)
    {
        const std::size_t start = i * size / submissions;
        const std::size_t end = (i + 1) * size / submissions;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            pool.submit(type_helper<ThreadCount, C, F>::worker, container.begin() + start, container.begin() + end, fun,
                        i);
    }
    pool.await_pending();
}

template <std::size_t ThreadCount, ValidContainer C, typename F>
void for_each(C &container, F fun, const std::vector<std::size_t> &splits)
{
    KIT_ASSERT_ERROR(splits.size() > 0, "Splits size must be greater than 0")
    auto &pool = type_helper<ThreadCount, C, F>::pool;
    const std::size_t size = container.size();

    for (std::size_t i = 0; i < splits.size(); i++)
    {
        const std::size_t start = i == 0 ? 0 : splits[i - 1];
        const std::size_t end = i == (splits.size() - 1) ? size : splits[i];
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            pool.submit(type_helper<ThreadCount, C, F>::worker, container.begin() + start, container.begin() + end, fun,
                        i);
    }
    pool.await_pending();
}

} // namespace kit::mt