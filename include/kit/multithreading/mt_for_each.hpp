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
    typename T::value_type;
    {
        a.size()
    } -> std::convertible_to<std::size_t>;
};

template <typename Ret> struct feach_return
{
    using type = std::vector<std::future<std::vector<Ret>>>;
};
template <> struct feach_return<void>
{
    using type = void;
};

template <typename C, typename F, class... Args> struct type_helper
{
    using iterator_t = decltype(std::declval<C>().begin());
    using fun_ret_t = std::invoke_result_t<F, std::size_t, typename C::value_type, Args...>;
    
    // doing this bc shitty msvc wont discard the right fucking if constexpr branch
    static void wimpl(iterator_t it1, iterator_t it2, F fun, const std::size_t workload_index, std::true_type, Args &&...args) 
    {
        for (auto it = it1; it != it2; ++it)
            fun(workload_index, *it, std::forward<Args>(args)...);
    }
    static auto wimpl(iterator_t it1, iterator_t it2, F fun, const std::size_t workload_index, std::false_type, Args &&...args) 
    {
        std::vector<fun_ret_t> ret;
        ret.reserve(std::distance(it1, it2));
        for (auto it = it1; it != it2; ++it)
            ret.push_back(fun(workload_index, *it, std::forward<Args>(args)...));
        return ret;
    }

    static inline const auto worker = [](iterator_t it1, iterator_t it2, F fun, const std::size_t workload_index,
                                         Args &&...args) 
    {
        return wimpl(it1, it2, fun, workload_index, std::is_same<fun_ret_t, void>{}, std::forward<Args>(args)...);
    };
};

template <ValidContainer C, typename F, class... Args>
auto for_each(thread_pool &pool, C &container, F fun, const std::size_t workloads, Args &&...args)
    -> feach_return<typename type_helper<C, F, Args...>::fun_ret_t>::type
{
    KIT_ASSERT_ERROR(workloads != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(workloads > 1, "Parallel for each purpose is defeated with only a single workload")

    using fun_ret_t = typename type_helper<C, F, Args...>::fun_ret_t;
    using feach_ret_t = typename feach_return<fun_ret_t>::type;

    const std::size_t size = container.size();
    if constexpr (std::is_same_v<fun_ret_t, void>)
    {
        for (std::size_t i = 0; i < workloads; i++)
        {
            const std::size_t start = i * size / workloads;
            const std::size_t end = (i + 1) * size / workloads;
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                pool.submit(type_helper<C, F, Args...>::worker, container.begin() + start, container.begin() + end, fun,
                            i, std::forward<Args>(args)...);
        }
        pool.await_pending();
    }
    else
    {
        feach_ret_t futures;
        futures.reserve(workloads);
        for (std::size_t i = 0; i < workloads; i++)
        {
            const std::size_t start = i * size / workloads;
            const std::size_t end = (i + 1) * size / workloads;
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                futures.push_back(pool.submit(type_helper<C, F, Args...>::worker, container.begin() + start,
                                              container.begin() + end, fun, i, std::forward<Args>(args)...));
        }
        return futures;
    }
}

template <ValidContainer C, typename F, class... Args>
auto for_each(thread_pool &pool, C &container, F fun, const std::vector<std::size_t> &splits, Args &&...args)
    -> feach_return<typename type_helper<C, F, Args...>::fun_ret_t>::type
{
    KIT_ASSERT_ERROR(splits.size() != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(splits.size() > 1, "Parallel for each purpose is defeated with only a single workload")

    using fun_ret_t = typename type_helper<C, F, Args...>::fun_ret_t;
    using feach_ret_t = typename feach_return<fun_ret_t>::type;
    const std::size_t size = container.size();
    if constexpr (std::is_same_v<fun_ret_t, void>)
    {
        for (std::size_t i = 0; i < splits.size(); i++)
        {
            const std::size_t start = i == 0 ? 0 : splits[i - 1];
            const std::size_t end = i == (splits.size() - 1) ? size : splits[i];
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                pool.submit(type_helper<C, F, Args...>::worker, container.begin() + start, container.begin() + end, fun,
                            i, std::forward<Args>(args)...);
        }
        pool.await_pending();
    }
    else
    {
        feach_ret_t futures;
        futures.reserve(splits.size());
        for (std::size_t i = 0; i < splits.size(); i++)
        {
            const std::size_t start = i == 0 ? 0 : splits[i - 1];
            const std::size_t end = i == (splits.size() - 1) ? size : splits[i];
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                futures.push_back(pool.submit(type_helper<C, F, Args...>::worker, container.begin() + start,
                                              container.begin() + end, fun, i, std::forward<Args>(args)...));
        }
        return futures;
    }
}

template <ValidContainer C, typename F, class... Args>
auto for_each_async(C &container, F fun, const std::size_t workloads, Args &&...args)
    -> feach_return<typename type_helper<C, F, Args...>::fun_ret_t>::type
{
    KIT_ASSERT_ERROR(workloads != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(workloads > 1, "Parallel for each purpose is defeated with only a single workload")

    using fun_ret_t = typename type_helper<C, F, Args...>::fun_ret_t;
    using feach_ret_t = typename feach_return<fun_ret_t>::type;

    const std::size_t size = container.size();

    feach_ret_t futures;
    futures.reserve(workloads);
    for (std::size_t i = 0; i < workloads; i++)
    {
        const std::size_t start = i * size / workloads;
        const std::size_t end = (i + 1) * size / workloads;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            futures.push_back(std::async(std::launch::async, type_helper<C, F, Args...>::worker,
                                         container.begin() + start, container.begin() + end, fun, i,
                                         std::forward<Args>(args)...));
    }
    if constexpr (std::is_same_v<fun_ret_t, void>)
        for (auto &future : futures)
            future.get();
    else
        return futures;
}

template <ValidContainer C, typename F, class... Args>
auto for_each_async(C &container, F fun, const std::vector<std::size_t> &splits, Args &&...args)
    -> feach_return<typename type_helper<C, F, Args...>::fun_ret_t>::type
{
    KIT_ASSERT_ERROR(splits.size() != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(splits.size() > 1, "Parallel for each purpose is defeated with only a single workload")

    using fun_ret_t = typename type_helper<C, F, Args...>::fun_ret_t;
    using feach_ret_t = typename feach_return<fun_ret_t>::type;

    const std::size_t size = container.size();

    feach_ret_t futures;
    futures.reserve(splits.size());
    for (std::size_t i = 0; i < splits.size(); i++)
    {
        const std::size_t start = i == 0 ? 0 : splits[i - 1];
        const std::size_t end = i == (splits.size() - 1) ? size : splits[i];
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        if (end > start)
            futures.push_back(std::async(std::launch::async, type_helper<C, F, Args...>::worker,
                                         container.begin() + start, container.begin() + end, fun, i,
                                         std::forward<Args>(args)...));
    }
    if constexpr (std::is_same_v<fun_ret_t, void>)
        for (auto &future : futures)
            future.get();
    else
        return futures;
}

} // namespace kit::mt