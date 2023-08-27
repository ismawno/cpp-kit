#ifndef KIT_BLOCK_ALLOCATOR_HPP
#define KIT_BLOCK_ALLOCATOR_HPP

#include "kit/debug/log.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

namespace kit
{

template <std::size_t BlockSize = 8 * 1024, std::size_t MaxChunkSize = 1024, std::uint32_t SupportedSizesIncrement = 32>
class block_allocator
{
    inline static constexpr std::size_t SUPPORTED_SIZES_COUNT = MaxChunkSize / SupportedSizesIncrement + 1;
    struct chunk
    {
        chunk *next = nullptr;
    };

    struct block
    {
        std::unique_ptr<std::byte[]> chunks = nullptr;
        std::size_t chunk_size = 0;
#ifdef DEBUG
        std::uint32_t dbg_alloc_count;
        bool owns_chunk(const chunk *ck)
        {
            const std::byte *ck_byte = (std::byte *)ck;
            const std::byte *first_chunk = chunks.get();
            const std::size_t chunk_count = BlockSize / chunk_size;

            for (std::size_t i = 0; i < chunk_count; i++)
                if (ck_byte == (first_chunk + i * chunk_size))
                    return true;
            return false;
        }
#endif
    };

    struct size_index_mapper
    {
        size_index_mapper()
        {
            for (std::size_t i = 0; i < SUPPORTED_SIZES_COUNT; i++)
                supported_chunk_sizes[i] = i * SupportedSizesIncrement;

            size_to_index[0] = 0;
            std::size_t mapped_index = 0;
            for (std::size_t size = 1; size <= MaxChunkSize; size++)
            {
                if (size > supported_chunk_sizes[mapped_index])
                    mapped_index++;
                size_to_index[size] = mapped_index;
            }
        }

        std::array<std::size_t, MaxChunkSize + 1> size_to_index;
        std::array<std::size_t, SUPPORTED_SIZES_COUNT> supported_chunk_sizes;
    };

    inline static const size_index_mapper s_mapper{};
    inline static std::vector<block> s_blocks{};
    inline static std::array<chunk *, SUPPORTED_SIZES_COUNT> s_free_chunks{};

  public:
    template <typename T> static T *allocate()
    {
        KIT_DEBUG("Block allocating {0} bytes of data", sizeof(T))
        const std::size_t mapped_index = s_mapper.size_to_index[sizeof(T)];

        if (s_free_chunks[mapped_index])
            return next_free_chunk<T>(mapped_index);
        return first_chunk_of_new_block<T>(mapped_index);
    }

    template <typename T> static void deallocate(T *ptr, const std::size_t size = sizeof(T))
    {
        KIT_ASSERT_CRITICAL(size > 0, "Attempting to deallocate a non-positive amount of memory: {0}", size)
        KIT_ASSERT_ERROR(ptr, "Attempting to deallocate null pointer in block allocator")
        KIT_ASSERT_CRITICAL(size <= MaxChunkSize,
                            "Attempting to deallocate a chunk with size greater that MaxChunkSize. size: {0}, max: {1}",
                            size, MaxChunkSize)
        KIT_DEBUG("Block deallocating {0} bytes of data", size)

        const std::size_t mapped_index = s_mapper.size_to_index[size];
        chunk *ck = (chunk *)ptr;
#ifdef DEBUG
        make_sure_chunk_belongs_to_allocator(mapped_index, ck, size);
#endif

        ck->next = s_free_chunks[mapped_index];
        s_free_chunks[mapped_index] = ck;
    }

    template <typename T> static constexpr bool can_allocate()
    {
        return sizeof(T) <= MaxChunkSize;
    }

    static bool can_deallocate(const std::size_t size)
    {
        return size <= MaxChunkSize;
    }

  private:
    template <typename T> static T *next_free_chunk(const std::size_t mapped_index)
    {
        chunk *ck = s_free_chunks[mapped_index];
        s_free_chunks[mapped_index] = ck->next;
#ifdef DEBUG
        const std::size_t chunk_size = s_mapper.supported_chunk_sizes[mapped_index];
        for (block &bk : s_blocks)
            if (bk.chunk_size == chunk_size && bk.owns_chunk(ck))
            {
                bk.dbg_alloc_count++;
                report_block(bk);
                break;
            }
#endif
        return (T *)ck;
    }

    template <typename T> static T *first_chunk_of_new_block(const std::size_t mapped_index)
    {
        const std::size_t chunk_size = s_mapper.supported_chunk_sizes[mapped_index];
        KIT_INFO("Creating new block at index {0} with size {1} and {2} bytes per chunk", mapped_index, BlockSize,
                 chunk_size)

        block &bk = s_blocks.emplace_back();
        bk.chunk_size = chunk_size;
        bk.chunks = std::unique_ptr<std::byte[]>(new std::byte[BlockSize]);

        std::byte *first_chunk = bk.chunks.get();
        const std::size_t chunk_count = BlockSize / chunk_size;
        KIT_ASSERT_ERROR(chunk_count * chunk_size <= BlockSize,
                         "Number of chunks times the size of each chunk is not equal (or less) than the chunk size!")

        for (std::size_t i = 0; i < chunk_count - 1; i++)
        {
            chunk *current = (chunk *)(first_chunk + i * chunk_size);
            current->next = (chunk *)(first_chunk + (i + 1) * chunk_size);
        }
        chunk *last = (chunk *)(first_chunk + (chunk_count - 1) * chunk_size);
        last->next = nullptr;

        s_free_chunks[mapped_index] = ((chunk *)first_chunk)->next;
#ifdef DEBUG
        bk.dbg_alloc_count = 1;
        report_blocks();
#endif
        return (T *)first_chunk;
    }

#ifdef DEBUG
    static void make_sure_chunk_belongs_to_allocator(const std::size_t mapped_index, const chunk *ck,
                                                     const std::size_t size)
    {
        const std::size_t chunk_size = s_mapper.supported_chunk_sizes[mapped_index];
        for (block &bk : s_blocks)
        {
            if (bk.chunk_size != chunk_size)
                continue;

            if (bk.owns_chunk(ck))
            {
                bk.dbg_alloc_count--;
                report_block(bk);
                return;
            }
        }
        KIT_CRITICAL("Pointer {0} with size {1} bytes was not found in any chunk of any blocks!", (void *)ck, size)
    }

    static void report_block(const block &bk)
    {
        std::size_t idx = 0;
        for (std::size_t i = 1; i < s_blocks.size(); i++)
            if (&bk == &(s_blocks[i]))
            {
                idx = i;
                break;
            }
        const std::size_t chunk_count = BlockSize / bk.chunk_size;

        KIT_DEBUG("Block {0}: {1} chunks, with {2} bytes per chunk, of which {3} ({4:.1f}%) are occupied ({5} bytes)",
                  idx++, chunk_count, bk.chunk_size, bk.dbg_alloc_count,
                  100.f * (float)bk.dbg_alloc_count / (float)chunk_count, bk.dbg_alloc_count * bk.chunk_size)
    }
    static void report_blocks()
    {
        KIT_INFO("There are currently {0} block(s) allocated, occupying {1} bytes each ({2} bytes total)",
                 s_blocks.size(), BlockSize, s_blocks.size() * BlockSize)
        std::size_t idx = 0;
        for (const block &bk : s_blocks)
        {
            const std::size_t chunk_count = BlockSize / bk.chunk_size;

            KIT_INFO(
                "Block {0}: {1} chunks, with {2} bytes per chunk, of which {3} ({4:.1f}%) are occupied ({5} bytes)",
                idx++, chunk_count, bk.chunk_size, bk.dbg_alloc_count,
                100.f * (float)bk.dbg_alloc_count / (float)chunk_count, bk.dbg_alloc_count * bk.chunk_size)
        }
    }
#endif
};

template <typename T, std::size_t BlockSize = 8 * 1024, std::size_t MaxChunkSize = 1024,
          std::uint32_t SupportedSizesIncrement = 32>
struct block_deleter
{
    constexpr block_deleter() noexcept : m_allocated_size(sizeof(T)){};

    template <typename U> block_deleter(const block_deleter<U> &bd) noexcept : m_allocated_size(bd.m_allocated_size)
    {
    }

    void operator()(T *p)
    {
        if (block_allocator<BlockSize, MaxChunkSize, SupportedSizesIncrement>::can_deallocate(m_allocated_size))
        {
            p->~T();
            block_allocator<BlockSize, MaxChunkSize, SupportedSizesIncrement>::deallocate(p, m_allocated_size);
        }
        else
            delete p;
    }

  private:
    std::size_t m_allocated_size;

    template <typename U, std::size_t BS, std::size_t MCS, std::uint32_t SSI> friend struct block_deleter;
};
} // namespace kit

#endif