#pragma once

// #include "kit/debug/log.hpp"
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>
#include <tuple>
#include <unordered_map>
#include <atomic>

namespace kit::mt
{
template <class... Args> class task
{
  public:
    using fun = std::function<void(Args...)>;
    task(const fun &fn, Args... args) : m_fun(fn), m_args(std::forward<Args>(args)...)
    {
    }

    void operator()() const
    {
        KIT_ASSERT_ERROR(m_fun, "The task must not be null")
        (*this)(std::make_integer_sequence<int, sizeof...(Args)>{});
    }

  private:
    fun m_fun;
    std::tuple<Args...> m_args;

    template <int... Seq> void operator()(std::integer_sequence<int, Seq...> seq) const
    {
        m_fun(std::get<Seq>(m_args)...);
    }
};
template <class... Args> class thread_pool
{
  public:
    thread_pool(const std::size_t thread_count)
    {
        const auto worker = [this]() {
            for (;;)
            {
                std::unique_lock<std::mutex> lock{m_mutex};

                m_check_task.wait(lock, [this]() { return !m_tasks.empty() || m_termination_signal; });
                if (m_tasks.empty())
                    break;

                const task<Args...> tsk = m_tasks.front();
                m_tasks.pop();
                lock.unlock();

                tsk();
                lock.lock();
                --m_pending_tasks;
                m_check_idle.notify_one();
            }
        };
        m_threads.reserve(thread_count);

        for (std::size_t i = 0; i < thread_count; i++)
            m_threads.emplace_back(worker);
    }

    ~thread_pool()
    {
        await_pending();
        {
            std::scoped_lock<std::mutex> lock{m_mutex};
            m_termination_signal = true;
            m_check_task.notify_all();
        }
        for (std::thread &th : m_threads)
        {
            if (th.joinable())
                th.join();
        }
    }

    void submit(const typename task<Args...>::fun &fn, Args... args)
    {
        std::scoped_lock<std::mutex> lock{m_mutex};
        ++m_pending_tasks;
        m_tasks.emplace(fn, std::forward<Args>(args)...);
        m_check_task.notify_one();
    }

    std::size_t size() const
    {
        return m_threads.size();
    }
    std::size_t unattended_tasks() const
    {
        return m_tasks.size();
    }
    std::size_t pending_tasks() const
    {
        return m_pending_tasks.load();
    }
    bool idle() const
    {
        return m_pending_tasks.load() == 0;
    }
    void await_pending()
    {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_check_idle.wait(lock, [this]() { return m_pending_tasks.load() == 0; });
    }

  private:
    std::vector<std::thread> m_threads;
    std::atomic<std::size_t> m_pending_tasks{0};

    std::queue<task<Args...>> m_tasks;
    std::mutex m_mutex;

    std::condition_variable m_check_task;
    std::condition_variable m_check_idle;
    bool m_termination_signal = false;

    thread_pool(const thread_pool &) = delete;
    thread_pool &operator=(const thread_pool &) = delete;
};
} // namespace kit::mt