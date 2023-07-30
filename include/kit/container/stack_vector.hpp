#ifndef KIT_STACK_VECTOR_HPP
#define KIT_STACK_VECTOR_HPP

#ifdef KIT_USE_CUSTOM_ALLOC
#include "kit/memory/stack_allocator.hpp"
#endif

namespace kit
{
#ifdef KIT_USE_CUSTOM_ALLOC
template <typename T, std::size_t Capacity = 1024, std::uint32_t ID = 0>
using stack_vector = std::vector<T, kit::stack_allocator<T, Capacity, ID>>;
#else
template <typename T, std::size_t Capacity = 1024, std::uint32_t ID = 0> using stack_vector = std::vector<T>;
#endif
} // namespace kit

#endif