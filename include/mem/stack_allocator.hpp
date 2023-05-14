#ifndef STACK_ALLOCATOR_HPP
#define STACK_ALLOCATOR_HPP

#include <functional>
#include <memory>
#include <array>
// #include <atomic>
#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif

#define MEM_STACK_CAPACITY 102400 // Change this so that it can be modified somehow
#define MEM_MAX_ENTRIES 16

namespace mem
{
    struct stack_entry
    {
        char *data = nullptr;
        bool used_default = false;
    };

    inline std::size_t _stack_size = 0, _entry_index = 0; // Set this atomic??
    inline std::array<stack_entry, MEM_MAX_ENTRIES> _stack_entries;
    inline char *_stack_buffer = nullptr;

    inline void preallocate_stack() { _stack_buffer = new char[MEM_STACK_CAPACITY]; }
    inline void free_stack() { delete[] _stack_buffer; }

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
            DBG_TRACE("Instantiated stack allocator.")
        }
        template <typename U>
        stack_allocator(const stack_allocator<U> &other) noexcept : base(other) {}

        template <typename U>
        struct rebind
        {
            using other = stack_allocator<U>;
        };

        ptr allocate(size n)
        {
            DBG_ASSERT_CRITICAL(_entry_index < (MEM_MAX_ENTRIES - 1), "No more entries available in stack allocator when trying to allocate {0} bytes! Maximum: {1}", n * sizeof(T), MEM_MAX_ENTRIES)

            const bool enough_space = _stack_size + n * sizeof(T) < MEM_STACK_CAPACITY;
            DBG_ASSERT_WARN(enough_space, "No more space available in stack allocator when trying to allocate {0} bytes! Maximum: {1} bytes", MEM_STACK_CAPACITY)
            _stack_entries[_entry_index].data = enough_space ? (_stack_buffer + _stack_size) : (char *)base::allocate(n);
            _stack_entries[_entry_index].used_default = !enough_space;
            ptr p = (ptr)_stack_entries[_entry_index].data;

            _entry_index++;
            _stack_size += n * sizeof(T);

            DBG_DEBUG("Stack allocating {0} bytes of data. {1} entries and {2} bytes remaining in buffer.", n * sizeof(T), MEM_MAX_ENTRIES - _entry_index, MEM_STACK_CAPACITY - _stack_size)
            return p;
        }

        void deallocate(ptr p, size n)
        {
            _entry_index--;
            DBG_ASSERT_CRITICAL(p == (ptr)_stack_entries[_entry_index].data, "Trying to deallocate disorderly from stack allocator!")
            if (_stack_entries[_entry_index].used_default)
                base::deallocate(p, n);
            else
                _stack_size -= n * sizeof(T);
            DBG_DEBUG("Stack deallocating {0} bytes of data. {1} entries and {2} bytes remaining in buffer.", n * sizeof(T), MEM_MAX_ENTRIES - _entry_index, MEM_STACK_CAPACITY - _stack_size)
        }
    };
}

#endif