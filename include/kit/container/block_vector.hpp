#ifndef KIT_BLOCK_VECTOR_HPP
#define KIT_BLOCK_VECTOR_HPP

#ifdef KIT_USE_CUSTOM_ALLOC
#include "kit/memory/block_allocator.hpp"
#endif

namespace kit
{
#ifdef KIT_USE_CUSTOM_ALLOC
template <typename T> using block_vector = std::vector<T, kit::block_allocator<T>>;
#else
template <typename T> using block_vector = std::vector<T>;
#endif
} // namespace kit

#endif