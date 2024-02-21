#include "kit/debug/log.hpp"
#include "kit/multithreading/thread_pool.hpp"
#include "kit/utility/type_constraints.hpp"
#include <thread>
#include <functional>

namespace kit::mt
{
template <typename It, typename Fun> void _for_each_worker(It it1, It it2, Fun fun, const std::size_t thread_idx)
{
    for (auto it = it1; it != it2; ++it)
        fun(thread_idx, *it);
}

template <RandomAccessContainer C, typename Fun>
using for_each_thread_pool =
    thread_pool<decltype(std::declval<C>().begin()), decltype(std::declval<C>().begin()), Fun, std::size_t>;

template <RandomAccessContainer C, typename Fun>
void for_each(C &container, Fun fun, const for_each_thread_pool<C, Fun> &pool)
{
    const std::size_t size = container.size();
    for (std::size_t i = 0; i < pool.size(); i++)
    {
        const std::size_t start = i * size / pool.size();
        const std::size_t end = (i + 1) * size / pool.size();
        KIT_ASSERT_ERROR(end <= size, "Partition exceeds vector size! start: {0}, end: {1}, size: {2}", start, end,
                         size)
        pool.submit(_for_each_worker<decltype(container.begin()), Fun>, container.begin() + start,
                    container.begin() + end, fun, i);
    }
    pool.await_pending();
}
} // namespace kit::mt