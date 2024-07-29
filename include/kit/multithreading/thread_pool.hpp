#pragma once

#include "kit/debug/log.hpp"
#include "kit/memory/ptr/ref.hpp"
#include "kit/utility/type_constraints.hpp"
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>
#include <future>
#include <type_traits>

namespace kit::mt
{
class thread_pool
{
  public:
    thread_pool(std::size_t thread_count);
    ~thread_pool();

    template <typename F, class... Args>
    auto submit(F &&fun, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_t = std::invoke_result_t<F, Args...>;

#ifndef _MSC_VER
        std::packaged_task<return_t()> task{std::bind(std::forward<F>(fun), std::forward<Args>(args)...)};
        std::future<return_t> future = task.get_future();
#else
        auto task =
            kit::make_ref<std::packaged_task<return_t()>>(std::bind(std::forward<F>(fun), std::forward<Args>(args)...));
        std::future<return_t> future = task->get_future();
#endif

        {
            std::scoped_lock<std::mutex> lock{m_mutex};
            m_pending_tasks++;
#ifndef _MSC_VER
            m_tasks.emplace(std::move(task));
#else
            m_tasks.push([task] { (*task)(); });
#endif
        }
        m_check_task.notify_one(); // move this into/out of the lock?
        return future;
    }

    std::size_t thread_count() const;
    std::size_t unattended_tasks() const;
    std::size_t pending_tasks() const;
    std::size_t thread_index() const;

    bool idle() const;
    void await_pending();

  private:
    std::vector<std::thread> m_threads;
    std::size_t m_pending_tasks = 0;

#ifndef _MSC_VER
    std::queue<std::packaged_task<void()>> m_tasks;
#else
    std::queue<std::function<void()>> m_tasks;
#endif
    std::mutex m_mutex;

    std::condition_variable m_check_task;
    std::condition_variable m_check_idle;
    bool m_termination_signal = false;

    thread_pool(const thread_pool &) = delete;
    thread_pool &operator=(const thread_pool &) = delete;
};

} // namespace kit::mt