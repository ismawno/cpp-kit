#pragma once

#include "kit/debug/log.hpp"
#include "kit/multithreading/thread_pool.hpp"
#include "kit/utility/type_constraints.hpp"
#include <thread>
#include <functional>

namespace kit::mt
{
template <typename Ret> struct feach_return
{
    using feach_t = std::vector<std::future<std::vector<Ret>>>;
    using feach_iter_t = std::vector<std::future<Ret>>;
};
template <> struct feach_return<void>
{
    using feach_t = void;
    using feach_iter_t = void;
};

template <typename It, typename F, class... Args> struct type_helper
{
    using fun_ret_t = std::invoke_result_t<F, typename It::value_type, Args...>;

    // doing this bc shitty msvc wont discard the right fucking if constexpr branch
    static void worker_impl(It it1, It it2, F &&fun, std::true_type, Args &&...args)
    {
        for (auto it = it1; it != it2; ++it)
            std::forward<F>(fun)(*it, std::forward<Args>(args)...);
    }
    static auto worker_impl(It it1, It it2, F &&fun, std::false_type, Args &&...args)
    {
        std::vector<fun_ret_t> ret;
        ret.reserve(std::distance(it1, it2));
        for (auto it = it1; it != it2; ++it)
            ret.push_back(std::forward<F>(fun)(*it, std::forward<Args>(args)...));
        return ret;
    }

    static inline const auto worker = [](It it1, It it2, F &&fun, Args &&...args) {
        return worker_impl(it1, it2, std::forward<F>(fun), std::is_same<fun_ret_t, void>{},
                           std::forward<Args>(args)...);
    };
};

template <std::random_access_iterator It, typename F, class... Args>
auto for_each(thread_pool &pool, It it1, It it2, F &&fun, const std::size_t workloads, Args &&...args)
    -> feach_return<typename type_helper<It, F, Args...>::fun_ret_t>::feach_t
{
    KIT_ASSERT_ERROR(workloads != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(workloads > 1, "Parallel for each purpose is defeated with only a single workload")

    using fun_ret_t = typename type_helper<It, F, Args...>::fun_ret_t;
    using feach_ret_t = typename feach_return<fun_ret_t>::feach_t;

    const std::size_t size = std::distance(it1, it2);
    std::size_t start = 0;
    if constexpr (std::is_same_v<fun_ret_t, void>)
    {
        for (std::size_t i = 0; i < workloads; i++)
        {
            const std::size_t end = (i + 1) * size / workloads;
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                pool.submit(type_helper<It, F, Args...>::worker, it1 + start, it1 + end, std::forward<F>(fun),
                            std::forward<Args>(args)...);
            start = end;
        }
        pool.await_pending();
    }
    else
    {
        feach_ret_t futures;
        futures.reserve(workloads);
        for (std::size_t i = 0; i < workloads; i++)
        {
            const std::size_t end = (i + 1) * size / workloads;
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                futures.push_back(pool.submit(type_helper<It, F, Args...>::worker, it1 + start, it1 + end,
                                              std::forward<F>(fun), std::forward<Args>(args)...));
            start = end;
        }
        return futures;
    }
}

template <typename It, typename F, class... Args> struct type_helper_iter
{
    using fun_ret_t = std::invoke_result_t<F, It, It, Args...>;
};

template <std::random_access_iterator It, typename F, class... Args>
auto for_each_iter(thread_pool &pool, It it1, It it2, F &&fun, const std::size_t workloads, Args &&...args)
    -> feach_return<typename type_helper_iter<It, F, Args...>::fun_ret_t>::feach_iter_t
{
    KIT_ASSERT_ERROR(workloads != 0, "Workload count must be greater than 0")
    KIT_ASSERT_WARN(workloads > 1, "Parallel for each purpose is defeated with only a single workload")

    using fun_ret_t = typename type_helper_iter<It, F, Args...>::fun_ret_t;
    using feach_iter_ret_t = typename feach_return<fun_ret_t>::feach_iter_t;

    const std::size_t size = std::distance(it1, it2);
    std::size_t start = 0;
    if constexpr (std::is_same_v<fun_ret_t, void>)
    {
        for (std::size_t i = 0; i < workloads; i++)
        {
            const std::size_t end = (i + 1) * size / workloads;
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                pool.submit(fun, it1 + start, it1 + end, std::forward<Args>(args)...);
            start = end;
        }
        pool.await_pending();
    }
    else
    {
        feach_iter_ret_t futures;
        futures.reserve(workloads);
        for (std::size_t i = 0; i < workloads; i++)
        {
            const std::size_t end = (i + 1) * size / workloads;
            KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                             size)
            if (end > start)
                futures.push_back(pool.submit(fun, it1 + start, it1 + end, std::forward<Args>(args)...));
            start = end;
        }
        return futures;
    }
}

} // namespace kit::mt