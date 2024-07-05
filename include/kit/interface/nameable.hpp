#pragma once

#include "kit/utility/type_constraints.hpp"

namespace kit
{
template <typename T>
concept ValidNameType = std::is_convertible_v<T, const char *> || std::is_convertible_v<T, std::string>;

template <ValidNameType T = const char *> class nameable
{
  public:
    using name_type = T;

    nameable(const T &name);
    const T &name() const;

  protected:
    T m_name;
};

template <typename T>
concept Nameable = requires() {
    typename T::name_type;
    std::is_base_of_v<kit::nameable<typename T::name_type>, T>;
};

} // namespace kit
