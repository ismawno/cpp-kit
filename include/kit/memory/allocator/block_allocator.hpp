#pragma once

#include "kit/memory/allocator/allocator.hpp"
#include <vector>

namespace kit
{
template <typename T> class block_allocator final : public discrete_allocator<T>
{
  public:
    block_allocator(const std::size_t block_obj_count = 1024)
        : m_block_obj_count(block_obj_count), m_block_capacity(aligned_capacity(block_obj_count * object_size()))
    {
    }
    block_allocator(block_allocator &&other)
        : m_blocks(std::move(other.m_blocks)), m_block_obj_count(other.m_block_obj_count),
          m_block_capacity(other.m_block_capacity), m_next_free_chunk(other.m_next_free_chunk)
    {
        other.m_next_free_chunk = nullptr;
        other.m_blocks.clear();
    }

    block_allocator &operator=(block_allocator &&other)
    {
        if (this == &other)
            return *this;
        for (T *block : m_blocks)
            discrete_allocator<T>::platform_aware_aligned_dealloc(block);
        m_blocks = std::move(other.m_blocks);
        m_block_obj_count = other.m_block_obj_count;
        m_block_capacity = other.m_block_capacity;
        m_next_free_chunk = other.m_next_free_chunk;
        other.m_next_free_chunk = nullptr;
        other.m_blocks.clear();

        return *this;
    }

    ~block_allocator()
    {
        for (T *block : m_blocks)
            discrete_allocator<T>::platform_aware_aligned_dealloc(block);
    }

    T *allocate() override
    {
        if (m_next_free_chunk)
            return from_next_free_chunk();
        return from_first_chunk_of_new_block();
    }

    void deallocate(T *ptr) override
    {
        KIT_ASSERT_ERROR(ptr, "Cannot deallocate a null pointer");
        KIT_ASSERT_ERROR(owns(ptr), "The pointer {0} does not belong to this allocator", (void *)ptr);

        chunk *current = (chunk *)ptr;
        current->next = m_next_free_chunk;
        m_next_free_chunk = current;
    }

    bool owns(const T *ptr) const override
    {
        for (T *block : m_blocks)
            if (ptr >= block && ptr < block + m_block_obj_count)
                return true;
        return false;
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
        std::byte *data = (std::byte *)discrete_allocator<T>::platform_aware_aligned_alloc(m_block_capacity, align);

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
        return discrete_allocator<T>::aligned_size(capacity, align);
    }
};
} // namespace kit