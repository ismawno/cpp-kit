#include <thread>
#include <functional>
#include "kit/debug/log.hpp"

namespace kit
{
namespace
{
template <typename It, typename T>
static void const_compute_mt(It it1, It it2, std::function<void(std::size_t, const T &)> func,
                             const std::size_t thread_idx)
{
    for (auto it = it1; it != it2; ++it)
        func(thread_idx, *it);
}

template <typename It, typename T>
static void compute_mt(It it1, It it2, std::function<void(std::size_t, T &)> func, const std::size_t thread_idx)
{
    for (auto it = it1; it != it2; ++it)
        func(thread_idx, *it);
}

} // namespace

template <std::size_t ThreadCount, typename T, template <typename...> typename C>
void const_for_each_mt(const C<T> &container, const std::function<void(std::size_t, const T &)> &func)
{
    std::array<std::thread, ThreadCount> threads;
    const std::size_t size = container.size();
    for (std::size_t i = 0; i < ThreadCount; i++)
    {
        const std::size_t start = i * size / ThreadCount;
        const std::size_t end = (i + 1) * size / ThreadCount;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)

        threads[i] = std::thread(const_compute_mt<typename C<T>::const_iterator, T>, container.begin() + (long)start,
                                 container.begin() + (long)end, func, i);
    }

    for (std::thread &th : threads)
        if (th.joinable())
            th.join();
}

template <std::size_t ThreadCount, typename T, template <typename...> typename C>
void for_each_mt(C<T> &container, const std::function<void(std::size_t, T &)> &func)
{
    std::array<std::thread, ThreadCount> threads;
    const std::size_t size = container.size();
    for (std::size_t i = 0; i < ThreadCount; i++)
    {
        const std::size_t start = i * size / ThreadCount;
        const std::size_t end = (i + 1) * size / ThreadCount;
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)

        threads[i] = std::thread(compute_mt<typename C<T>::iterator, T>, container.begin() + (long)start,
                                 container.begin() + (long)end, func, i);
    }

    for (std::thread &th : threads)
        if (th.joinable())
            th.join();
}
} // namespace kit