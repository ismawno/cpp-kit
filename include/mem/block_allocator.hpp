#ifndef BLOCK_ALLOCATOR_HPP
#define BLOCK_ALLOCATOR_HPP

#define MEM_MAX_BLOCK_SIZE 512
#define MEM_SUPPORTED_SIZES_INCREMENT 8
#define MEM_SUPPORTED_SIZES_COUNT (MEM_MAX_BLOCK_SIZE / MEM_SUPPORTED_SIZES_INCREMENT)
#define MEM_CHUNK_SIZE (8 * 1024)

#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif
#include "mem/core.hpp"

#include <vector>
#include <cstdint>
#include <array>
#include <memory>

namespace mem
{
    using byte = std::uint8_t;
    struct size_helper
    {
        size_helper()
        {
            for (std::size_t i = 0; i < MEM_SUPPORTED_SIZES_COUNT; i++)
                supported_sizes[i] = (i + 1) * MEM_SUPPORTED_SIZES_INCREMENT;

            clamped_indices[0] = 0;
            std::size_t mapped_index = 0;
            for (std::size_t size = 1; size <= MEM_MAX_BLOCK_SIZE; size++)
            {
                if (size > supported_sizes[mapped_index])
                    mapped_index++;
                clamped_indices[size] = mapped_index;
            }
        }
        std::array<std::size_t, MEM_MAX_BLOCK_SIZE + 1> clamped_indices;
        std::array<std::size_t, MEM_SUPPORTED_SIZES_COUNT> supported_sizes;
    };

    struct block
    {
        block *next = nullptr;
    };

    struct chunk
    {
        std::size_t block_size = 0;
        std::unique_ptr<byte[]> blocks = nullptr;
    };

    inline std::vector<chunk> _chunks;
    inline std::vector<block *> _free_blocks(MEM_SUPPORTED_SIZES_COUNT, nullptr);
    inline const size_helper _helper;

    template <typename T>
    class block_allocator : public std::allocator<T>
    {
    private:
        using base = std::allocator<T>;
        using ptr = typename std::allocator_traits<base>::pointer;
        using size = typename std::allocator_traits<base>::size_type;

    public:
        block_allocator() noexcept
        {
            DBG_TRACE("Instantiated block allocator.")
        }
        template <typename U>
        block_allocator(const block_allocator<U> &other) noexcept : base(other)
        {
            DBG_TRACE("Instantiated block allocator.")
        }

        template <typename U>
        struct rebind
        {
            using other = block_allocator<U>;
        };

        ptr allocate(size n)
        {
            const std::size_t n_bytes = n * sizeof(T);
            DBG_ASSERT_CRITICAL(n > 0, "Attempting to allocate a non-positive amount of memory: {0}", n_bytes)
            DBG_DEBUG("Block allocating {0} bytes of data", n_bytes)
            if (n_bytes > MEM_MAX_BLOCK_SIZE)
                return base::allocate(n);

            const std::size_t clamped_index = _helper.clamped_indices[n_bytes],
                              clamped_size = _helper.supported_sizes[clamped_index];
            if (_free_blocks[clamped_index])
            {
                DBG_DEBUG("Found free block at index {0} with size {1}", clamped_index, clamped_size)
                block *b = _free_blocks[clamped_index];
                _free_blocks[clamped_index] = b->next;
                return (ptr)b;
            }
            DBG_DEBUG("Creating new chunk at index {0} with size {1} and {2} bytes per block", clamped_index, MEM_CHUNK_SIZE, clamped_size)

            chunk &ck = _chunks.emplace_back();
            ck.block_size = clamped_size;
            ck.blocks = std::unique_ptr<byte[]>(new byte[MEM_CHUNK_SIZE]);

            byte *first_block = ck.blocks.get();
            const std::size_t block_count = MEM_CHUNK_SIZE / clamped_size;
            DBG_ASSERT_ERROR(block_count * clamped_size <= MEM_CHUNK_SIZE, "Number of blocks times the size of each block is not equal (or less) than the chunk size!")

            for (std::size_t i = 0; i < block_count - 1; i++)
            {
                block *current = (block *)(first_block + i * clamped_size);
                current->next = (block *)(first_block + (i + 1) * clamped_size);
            }
            block *last = (block *)(first_block + (block_count - 1) * clamped_size);
            last->next = nullptr;

            _free_blocks[clamped_index] = ((block *)first_block)->next;
            return (ptr)first_block;
        }

        void deallocate(ptr p, size n)
        {
            if (!p)
            {
                DBG_WARN("Attempting to deallocate null pointer!")
                return;
            }
            const std::size_t n_bytes = n * sizeof(T);
            DBG_ASSERT_CRITICAL(n > 0, "Attempting to deallocate a non-positive amount of memory: {0}", n_bytes)
            DBG_DEBUG("Block deallocating {0} bytes of data", n_bytes)
            if (n_bytes > MEM_MAX_BLOCK_SIZE)
                return base::deallocate(p, n);
            const std::size_t clamped_index = _helper.clamped_indices[n_bytes];

#ifdef DEBUG
            const std::size_t clamped_size = _helper.supported_sizes[clamped_index];
            bool found = false;
            for (const auto &chunk : _chunks)
            {
                const byte *p_byte = (byte *)p;
                const bool overlaps_chunk = (p_byte + clamped_size) > chunk.blocks.get() &&
                                            (chunk.blocks.get() + MEM_CHUNK_SIZE) > p_byte;
                DBG_ASSERT_CRITICAL(!(chunk.block_size != clamped_size && overlaps_chunk), "Pointer {0} with size {1} bytes belongs (or overlaps) the wrong chunk!", (void *)p, n_bytes)

                const bool belongs_to_chunk = chunk.blocks.get() <= p_byte &&
                                              (p_byte + clamped_size) <= (chunk.blocks.get() + MEM_CHUNK_SIZE);
                DBG_ASSERT_CRITICAL(!(chunk.block_size != clamped_size && belongs_to_chunk), "Pointer {0} with size {1} bytes belongs to the wrong chunk!", (void *)p, n_bytes)
                if (belongs_to_chunk)
                {
                    found = true;
                    break;
                }
            }
            DBG_ASSERT_CRITICAL(found, "Pointer {0} with size {1} bytes was not found in any block of any chunks!", (void *)p, n_bytes)
#endif
            block *b = (block *)p;
            b->next = _free_blocks[clamped_index];
            _free_blocks[clamped_index] = b;
        }
    };

    template <typename T>
    struct block_deleter
    {
        void operator()(T *p)
        {
            static block_allocator<T> alloc;
            alloc.deallocate(p, 1);
        }
    };
}

#endif