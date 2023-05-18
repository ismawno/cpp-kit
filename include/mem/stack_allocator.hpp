#ifndef STACK_ALLOCATOR_HPP
#define STACK_ALLOCATOR_HPP

#include <functional>
#include <memory>
#include <array>
#include <cstdint>
// #include <atomic>
#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif

#include "mem/core.hpp"
#include <mutex>

#define MEM_STACK_CAPACITY (64 * 1024) // Change this so that it can be modified somehow
#define MEM_MAX_ENTRIES 64

namespace mem
{
    using byte = std::uint8_t;

    class sdata
    {
        inline static std::size_t s_size = 0, s_index = 0;
        inline static std::array<byte *, MEM_MAX_ENTRIES> s_entries;
        inline static std::unique_ptr<byte[]> s_buffer = nullptr;
        inline static std::mutex s_mtx;

        static void preallocate() { s_buffer = std::unique_ptr<byte[]>(new byte[MEM_STACK_CAPACITY]); }
        template <typename T>
        friend class stack_allocator;
    };

    template <typename T>
    class stack_allocator : public std::allocator<T>
    {
    private:
        using base = std::allocator<T>;
        using ptr = typename std::allocator_traits<base>::pointer;
        using size = typename std::allocator_traits<base>::size_type;

    public:
        stack_allocator() noexcept
        {
            if (!sdata::s_buffer)
                sdata::preallocate();
            DBG_TRACE("Instantiated stack allocator.")
        }
        template <typename U>
        stack_allocator(const stack_allocator<U> &other) noexcept : stack_allocator()
        {
            if (!sdata::s_buffer)
                sdata::preallocate();
            DBG_TRACE("Instantiated stack allocator.")
        }

        template <typename U>
        struct rebind
        {
            using other = stack_allocator<U>;
        };

        ptr allocate_raw(size n_bytes) const noexcept
        {
            DBG_ASSERT_CRITICAL(n_bytes > 0, "Attempting to allocate a non-positive amount of memory: {0}", n_bytes)
            DBG_ASSERT_WARN(has_enough_entries(), "No more entries available in stack allocator when trying to allocate {0} bytes! Maximum entries: {1}", n_bytes, MEM_MAX_ENTRIES)
            DBG_ASSERT_WARN(has_enough_space(n_bytes), "No more space available in stack allocator when trying to allocate {0} bytes! Capacity: {1} bytes, amount used: {2}", n_bytes, MEM_STACK_CAPACITY, sdata::s_size)
            if (!has_enough_entries() || !has_enough_space(n_bytes))
                return nullptr;

            std::scoped_lock lock(sdata::s_mtx);

            sdata::s_entries[sdata::s_index] = sdata::s_buffer.get() + sdata::s_size;
            ptr p = (ptr)sdata::s_entries[sdata::s_index];

            sdata::s_index++;
            sdata::s_size += n_bytes;

            DBG_TRACE("Stack allocating {0} bytes of data. {1} entries and {2} bytes remaining in buffer.", n_bytes, MEM_MAX_ENTRIES - sdata::s_index, MEM_STACK_CAPACITY - sdata::s_size)
            return p;
        }

        bool deallocate_raw(ptr p, size n_bytes, const bool destroy_manually = false) const noexcept
        {
            DBG_ASSERT_CRITICAL(n_bytes > 0, "Attempting to deallocate a non-positive amount of memory: {0}", n_bytes)
            if (!p)
            {
                DBG_WARN("Attempting to deallocate null pointer!")
                return false;
            }

#ifdef DEBUG
            for (std::size_t i = sdata::s_index - 2; i >= 0 && i < sdata::s_entries.size(); i--)
            {
                DBG_ASSERT_CRITICAL(p != (ptr)sdata::s_entries[i], "Attempting to deallocate disorderly from stack! Entry is {0} positions away from top.", sdata::s_index - i)
            }
#endif
            if (sdata::s_index == 0 || p != (ptr)sdata::s_entries[sdata::s_index - 1])
                return false;

            if (destroy_manually)
                p->~T();
            std::scoped_lock lock(sdata::s_mtx);
            sdata::s_index--;
            sdata::s_size -= n_bytes;
            DBG_TRACE("Stack deallocating {0} bytes of data. {1} entries and {2} bytes remaining in buffer.", n_bytes, MEM_MAX_ENTRIES - sdata::s_index, MEM_STACK_CAPACITY - sdata::s_size)
            return true;
        }

        ptr allocate(size n)
        {
            const size n_bytes = n * sizeof(T);
            ptr p = allocate_raw(n_bytes);
            if (!p)
                return base::allocate(n);
            return p;
        }

        void deallocate(ptr p, size n)
        {
            const size n_bytes = n * sizeof(T);
            if (!deallocate_raw(p, n_bytes))
                base::deallocate(p, n);
        }

        bool has_enough_entries() const { return sdata::s_index < (MEM_MAX_ENTRIES - 1); }
        bool has_enough_space(const size n_bytes) const { return (sdata::s_size + n_bytes) < MEM_STACK_CAPACITY; }
    };

    // Only works for single allocations. It is intended to be used with smart pointers that default to new/delete if there is not enough memory
    template <typename T>
    struct stack_deleter
    {
        constexpr stack_deleter() noexcept : m_size(sizeof(T)){};

        template <typename U>
        stack_deleter(const stack_deleter<U> &bd) noexcept : m_size(bd.m_size) {}

        void operator()(T *p)
        {
            stack_allocator<T> alloc;
            if (!alloc.deallocate_raw(p, m_size, true))
                delete p;
        }

    private:
        std::size_t m_size = 0;

        template <typename U>
        friend struct stack_deleter;
    };
}

#endif