#pragma once

#include "kit/debug/log.hpp"
#include <vector>
#include <cstdlib>

namespace kit
{
template <typename T> class block_allocator
{
  public:
    block_allocator(const std::size_t block_obj_count = 256)
        : m_block_obj_count(block_obj_count), m_block_capacity(aligned_capacity(block_obj_count * object_size()))
    {
    }
    ~block_allocator()
    {
        for (T *block : m_blocks)
            std::free(block);
    }

    T *allocate()
    {
        if (m_next_free_chunk)
            return from_next_free_chunk();
        return from_first_chunk_of_new_block();
    }

    template <class... Args> T *create(Args &&...args)
    {
        T *ptr = allocate();
        new (ptr) T(std::forward<Args>(args)...);
        return ptr;
    }

    void destroy(T *ptr)
    {
        KIT_ASSERT_ERROR(ptr, "Cannot destroy a null pointer");
        ptr->~T();
        deallocate(ptr);
    }

    void deallocate(T *ptr)
    {
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
#ifdef DEBUG
        make_sure_ptr_belongs_to_allocator(ptr);
#endif
        chunk *current = (chunk *)ptr;
        current->next = m_next_free_chunk;
        m_next_free_chunk = current;
    }

  private:
    struct chunk
    {
        chunk *next;
    };

    std::vector<T *> m_blocks;

    std::size_t m_block_obj_count;
    std::size_t m_block_capacity;

    chunk *m_next_free_chunk = nullptr;

    T *from_first_chunk_of_new_block()
    {
        constexpr std::size_t align = alignment();
        constexpr std::size_t size = object_size();
        std::byte *data = (std::byte *)std::aligned_alloc(align, m_block_capacity);
        KIT_ASSERT_ERROR(data, "Failed to allocate memory for block");

        m_next_free_chunk = (chunk *)(data + size);
        for (std::size_t i = 0; i < m_block_obj_count - 1; i++)
        {
            chunk *current = (chunk *)(data + i * size);
            current->next = (chunk *)(data + (i + 1) * size);
        }
        chunk *last = (chunk *)(data + (m_block_obj_count - 1) * size);
        last->next = nullptr;

        m_blocks.push_back((T *)data);
        return (T *)data;
    }

    T *from_next_free_chunk()
    {
        chunk *current = m_next_free_chunk;
        m_next_free_chunk = current->next;
        return (T *)(current);
    }

    void make_sure_ptr_belongs_to_allocator(T *ptr)
    {
        for (T *block : m_blocks)
            if (ptr >= block && ptr < block + m_block_obj_count)
                return;
        KIT_CRITICAL("The pointer {0} does not belong to this allocator", (void *)ptr);
    }

    static inline constexpr std::size_t alignment()
    {
        return alignof(T) < alignof(chunk) ? alignof(chunk) : alignof(T);
    }
    static inline constexpr std::size_t object_size()
    {
        return sizeof(T) < sizeof(chunk) ? sizeof(chunk) : sizeof(T);
    }

    static inline constexpr std::size_t aligned_capacity(const std::size_t capacity)
    {
        constexpr std::size_t align = alignment();
        const std::size_t remainder = capacity % align;
        if (remainder == 0)
            return capacity;
        return capacity + align - remainder;
    }
};
} // namespace kit