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
    inline std::mutex mtx;
    struct stack_entry
    {
        byte *data = nullptr;
        bool used_default = false;
    };

    inline std::size_t _stack_size = 0, _entry_index = 0; // Set this atomic??
    inline std::array<stack_entry, MEM_MAX_ENTRIES> _stack_entries;
    inline std::unique_ptr<byte[]> _stack_buffer = nullptr;

    inline void preallocate_stack() { _stack_buffer = std::unique_ptr<byte[]>(new byte[MEM_STACK_CAPACITY]); }

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
            if (!_stack_buffer)
                preallocate_stack();
            DBG_TRACE("Instantiated stack allocator.")
        }
        template <typename U>
        stack_allocator(const stack_allocator<U> &other) noexcept : stack_allocator()
        {
            if (!_stack_buffer)
                preallocate_stack();
            DBG_TRACE("Instantiated stack allocator.")
        }

        template <typename U>
        struct rebind
        {
            using other = stack_allocator<U>;
        };

        ptr allocate(size n)
        {
            const std::size_t n_bytes = n * sizeof(T);
            DBG_ASSERT_CRITICAL(n_bytes > 0, "Attempting to allocate a non-positive amount of memory: {0}", n_bytes)
            DBG_ASSERT_CRITICAL(_entry_index < (MEM_MAX_ENTRIES - 1), "No more entries available in stack allocator when trying to allocate {0} bytes! Maximum: {1}", n_bytes, MEM_MAX_ENTRIES)

            std::scoped_lock lock(mtx);
            const bool enough_space = (_stack_size + n_bytes) < MEM_STACK_CAPACITY;
            DBG_ASSERT_WARN(enough_space, "No more space available in stack allocator when trying to allocate {0} bytes! Capacity: {1} bytes, amount used: {2}", MEM_STACK_CAPACITY, _stack_size)
            _stack_entries[_entry_index].data = enough_space ? (_stack_buffer.get() + _stack_size) : (byte *)base::allocate(n);
            _stack_entries[_entry_index].used_default = !enough_space;
            ptr p = (ptr)_stack_entries[_entry_index].data;

            _entry_index++;
            _stack_size += n_bytes;

            DBG_TRACE("Stack allocating {0} bytes of data. {1} entries and {2} bytes remaining in buffer.", n_bytes, MEM_MAX_ENTRIES - _entry_index, MEM_STACK_CAPACITY - _stack_size)
            return p;
        }

        void deallocate(ptr p, size n)
        {
            const std::size_t n_bytes = n * sizeof(T);
            DBG_ASSERT_CRITICAL(n_bytes > 0, "Attempting to deallocate a non-positive amount of memory: {0}", n_bytes)
            std::scoped_lock lock(mtx);
            _entry_index--;
            DBG_ASSERT_CRITICAL(p == (ptr)_stack_entries[_entry_index].data, "Trying to deallocate disorderly from stack allocator!")
            if (_stack_entries[_entry_index].used_default)
                base::deallocate(p, n);
            else
                _stack_size -= n_bytes;
            DBG_TRACE("Stack deallocating {0} bytes of data. {1} entries and {2} bytes remaining in buffer.", n_bytes, MEM_MAX_ENTRIES - _entry_index, MEM_STACK_CAPACITY - _stack_size)
        }
    };

    // Only works for single allocations
    template <typename T>
    struct stack_deleter
    {
        constexpr stack_deleter() noexcept : m_size(sizeof(T)){};

        template <typename U>
        stack_deleter(const stack_deleter<U> &bd) noexcept : m_size(bd.m_size) {}

        void operator()(T *p)
        {
            stack_allocator<T> alloc;
            p->~T();
            alloc.deallocate(p, 1);
        }

    private:
        std::size_t m_size = 0;

        template <typename U>
        friend struct stack_deleter;
    };
}

#endif