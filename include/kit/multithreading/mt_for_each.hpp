#include <thread>
#include <functional>
#include "kit/debug/log.hpp"

namespace kit::mt
{
template <typename It, typename Fun> void _for_each_worker(It it1, It it2, Fun fun, const std::size_t thread_idx)
{
    for (auto it = it1; it != it2; ++it)
        fun(thread_idx, *it);
}

template <std::size_t ThreadCount, typename C, typename Fun> void for_each_mt(C &container, Fun fun)
{
    std::array<std::thread, ThreadCount> threads;
    const std::size_t size = container.size();
    for (std::size_t i = 0; i < ThreadCount; i++)
    {
        const std::size_t start = i * size / ThreadCount;
        const std::size_t end = (i + 1) * size / ThreadCount;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)

        threads[i] = std::thread(_for_each_worker<decltype(container.begin()), Fun>, container.begin() + start,
                                 container.begin() + end, fun, i);
    }

    for (std::thread &th : threads)
        th.join();
}
} // namespace kit::mt