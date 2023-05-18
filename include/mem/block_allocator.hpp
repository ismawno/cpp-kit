#ifndef BLOCK_ALLOCATOR_HPP
#define BLOCK_ALLOCATOR_HPP

#define MEM_MAX_BLOCK_SIZE 2048
#define MEM_SUPPORTED_SIZES_INCREMENT 32
#define MEM_SUPPORTED_SIZES_COUNT (MEM_MAX_BLOCK_SIZE / MEM_SUPPORTED_SIZES_INCREMENT)
#define MEM_CHUNK_SIZE (16 * 1024)

#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif
#include "mem/core.hpp"

#include <vector>
#include <cstdint>
#include <array>
#include <memory>
#include <mutex>

namespace mem
{
    using byte = std::uint8_t;
    inline std::mutex mtx;
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
#ifdef DEBUG
        block *last = nullptr;
        std::size_t alloc_count;
#endif
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

        ptr allocate_raw(size n_bytes) const noexcept
        {
            DBG_ASSERT_CRITICAL(n_bytes > 0, "Attempting to allocate a non-positive amount of memory: {0}", n_bytes)
            DBG_DEBUG("Block allocating {0} bytes of data", n_bytes)
            if (n_bytes > MEM_MAX_BLOCK_SIZE)
                return nullptr;

            std::scoped_lock lock(mtx);
            const std::size_t clamped_index = _helper.clamped_indices[n_bytes];
            if (_free_blocks[clamped_index])
                return next_free_block(clamped_index);
            return first_block_of_new_chunk(clamped_index);
        }

        bool deallocate_raw(ptr p, size n_bytes) const noexcept
        {
            if (!p)
            {
                DBG_WARN("Attempting to deallocate null pointer!")
                return false;
            }

            DBG_ASSERT_CRITICAL(n_bytes > 0, "Attempting to deallocate a non-positive amount of memory: {0}", n_bytes)
            DBG_DEBUG("Block deallocating {0} bytes of data", n_bytes)
            if (n_bytes > MEM_MAX_BLOCK_SIZE)
                return false;

            std::scoped_lock lock(mtx);
            const std::size_t clamped_index = _helper.clamped_indices[n_bytes];
#ifdef DEBUG
            make_sure_block_belongs_to_allocator(clamped_index, p, n_bytes);
#endif
            block *b = (block *)p;
            b->next = _free_blocks[clamped_index];
            _free_blocks[clamped_index] = b;
            return true;
        }

        ptr allocate(size n)
        {
            // DBG_ASSERT_WARN(n <= 1, "Allocating more than one object at a time with block allocator. This is discouraged: the current implementation is intended to be used for single allocations (may change in the future)")
            const std::size_t n_bytes = n * sizeof(T);
            ptr p = allocate_raw(n_bytes);
            if (!p)
                return base::allocate(n);
            return p;
        }

        void deallocate(ptr p, size n) noexcept
        {
            const std::size_t n_bytes = n * sizeof(*p);
            if (!deallocate_raw(p, n_bytes))
                return base::deallocate(p, n);
        }

        ptr next_free_block(const std::size_t idx) const
        {
            block *b = _free_blocks[idx];
            _free_blocks[idx] = b->next;
#ifdef DEBUG
            for (chunk &ck : _chunks)
                if (ck.last == b)
                {
                    ck.alloc_count++;
                    ck.last = b->next;
                    report_chunk(ck);
                    break;
                }
#endif
            return (ptr)b;
        }

        ptr first_block_of_new_chunk(const std::size_t idx) const
        {
            const std::size_t clamped_size = _helper.supported_sizes[idx];
            DBG_INFO("Creating new chunk at index {0} with size {1} and {2} bytes per block", idx, MEM_CHUNK_SIZE, clamped_size)

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

            _free_blocks[idx] = ((block *)first_block)->next;
#ifdef DEBUG
            ck.alloc_count = 1;
            ck.last = _free_blocks[idx];
            report_chunks();
#endif
            return (ptr)first_block;
        }

#ifdef DEBUG
        void make_sure_block_belongs_to_allocator(const std::size_t idx, ptr p, const std::size_t n_bytes) const
        {
            const std::size_t clamped_size = _helper.supported_sizes[idx];
            bool found = false;
            for (chunk &ck : _chunks)
            {
                const byte *p_byte = (byte *)p;
                const bool overlaps_chunk = (p_byte + clamped_size) > ck.blocks.get() &&
                                            (ck.blocks.get() + MEM_CHUNK_SIZE) > p_byte;
                DBG_ASSERT_CRITICAL(!(ck.block_size != clamped_size && overlaps_chunk), "Pointer {0} with size {1} bytes belongs (or overlaps) the wrong chunk!", (void *)p, n_bytes)

                const bool belongs_to_chunk = ck.blocks.get() <= p_byte &&
                                              (p_byte + clamped_size) <= (ck.blocks.get() + MEM_CHUNK_SIZE);
                DBG_ASSERT_CRITICAL(!(ck.block_size != clamped_size && belongs_to_chunk), "Pointer {0} with size {1} bytes belongs to the wrong chunk!", (void *)p, n_bytes)
                if (belongs_to_chunk)
                {
                    found = true;
                    ck.alloc_count--;
                    ck.last = (block *)p;
                    report_chunk(ck);
                    break;
                }
            }
            DBG_ASSERT_CRITICAL(found, "Pointer {0} with size {1} bytes was not found in any block of any chunks!", (void *)p, n_bytes)
        }

        void report_chunks() const
        {
            DBG_INFO("There are currently {0} chunk(s) allocated, occupying {1} bytes each ({2} bytes total)", _chunks.size(), MEM_CHUNK_SIZE, _chunks.size() * MEM_CHUNK_SIZE)
            std::size_t idx = 0;
            for (const chunk &ck : _chunks)
            {
                const std::size_t block_count = MEM_CHUNK_SIZE / ck.block_size;
                DBG_INFO("Chunk {0}: {1} blocks, with {2} bytes per block, of which {3} are occupied ({4} bytes)", idx++, block_count, ck.block_size, ck.alloc_count, ck.alloc_count * ck.block_size)
            }
        }

        void report_chunk(const chunk &ck) const
        {
            std::size_t idx = 0;
            for (std::size_t i = 1; i < _chunks.size(); i++)
                if (&ck == &(_chunks[i]))
                {
                    idx = i;
                    break;
                }
            const std::size_t block_count = MEM_CHUNK_SIZE / ck.block_size;
            DBG_DEBUG("Chunk {0} reported with {1} blocks and {2} bytes per block, of which {3} are occupied ({4} bytes)", idx, block_count, ck.block_size, ck.alloc_count, ck.alloc_count * ck.block_size)
        }
#endif
    };

    // Only works for single allocations
    template <typename T>
    struct block_deleter
    {
        constexpr block_deleter() noexcept : m_size(sizeof(T)){};

        template <typename U>
        block_deleter(const block_deleter<U> &bd) noexcept : m_size(bd.m_size) {}

        void operator()(T *p)
        {
            block_allocator<T> alloc;
            if (m_size > MEM_MAX_BLOCK_SIZE)
                delete p;
            else
            {
                p->~T();
                alloc.deallocate_raw(p, m_size);
            }
        }

    private:
        std::size_t m_size = 0;

        template <typename U>
        friend struct block_deleter;
    };
}

#endif