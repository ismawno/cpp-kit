#include "kit/internal/pch.hpp"
#include "kit/multithreading/thread_pool.hpp"

namespace kit::mt
{
thread_pool::thread_pool(const std::size_t pool_size)
{
    const auto worker = [this]() {
        for (;;)
        {
            std::unique_lock<std::mutex> lock{m_mutex};

            m_check_task.wait(lock, [this]() { return !m_tasks.empty() || m_termination_signal; });
            if (m_tasks.empty())
                break;

            auto task = std::move(m_tasks.front());
            m_tasks.pop();

            lock.unlock();
            task();
            lock.lock();

            if (--m_pending_tasks == 0)
                m_check_idle.notify_one();
        }
    };
    for (std::size_t i = 0; i < pool_size; i++)
        m_threads.emplace_back(worker);
}

thread_pool::~thread_pool()
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

std::size_t thread_pool::thread_count() const
{
    return m_threads.size();
}
std::size_t thread_pool::unattended_tasks() const
{
    return m_tasks.size();
}
std::size_t thread_pool::pending_tasks() const
{
    return m_pending_tasks;
}
bool thread_pool::idle() const
{
    return m_pending_tasks == 0;
}
void thread_pool::await_pending()
{
    std::unique_lock<std::mutex> lock{m_mutex};
    m_check_idle.wait(lock, [this]() { return m_pending_tasks == 0; });
}

} // namespace kit::mt