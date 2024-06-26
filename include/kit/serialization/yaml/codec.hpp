#pragma once

#include <yaml-cpp/yaml.h>

namespace kit::yaml
{
template <typename T> struct codec;

class encodeable
{
#ifdef KIT_USE_YAML_CPP
  public:
    virtual ~encodeable() = default;
    virtual YAML::Node encode() const = 0;
#endif
};

class decodeable
{
#ifdef KIT_USE_YAML_CPP
  public:
    virtual ~decodeable() = default;
    virtual bool decode(const YAML::Node &node) = 0;
#endif
};

class codecable : public encodeable, public decodeable
{
#ifdef KIT_USE_YAML_CPP
  public:
    virtual ~codecable() = default;
#endif
};

#ifdef KIT_USE_YAML_CPP
template <typename T>
concept Encodeable = std::is_base_of_v<encodeable, T> || requires(T t) {
    {
        codec<T>::encode(t)
    } -> std::convertible_to<YAML::Node>;
};

template <typename T>
concept Decodeable = std::is_base_of_v<decodeable, T> || requires(T t, const YAML::Node &node) {
    {
        codec<T>::decode(node, t)
    } -> std::convertible_to<bool>;
};

template <typename T>
concept Codecable = Encodeable<T> && Decodeable<T>;

template <typename T> YAML::Emitter &operator<<(YAML::Emitter &out, const T &instance)
{
    out << YAML::BeginMap;
    if constexpr (std::is_base_of_v<encodeable, T>)
        out << instance.encode();
    else
        out << codec<T>::encode(instance);

    out << YAML::EndMap;
    return out;
}
#endif
} // namespace kit::yaml

#ifdef KIT_USE_YAML_CPP
template <typename T> struct YAML::convert
{
    static YAML::Node encode(const T &instance)
    {
        if constexpr (std::is_base_of_v<kit::yaml::encodeable, T>)
            return instance.encode();
        else
            return kit::yaml::codec<T>::encode(instance);
    }
    static bool decode(const YAML::Node &node, T &instance)
    {
        if constexpr (std::is_base_of_v<kit::yaml::decodeable, T>)
            return instance.decode(node);
        else
            return kit::yaml::codec<T>::decode(node, instance);
    }
};
#endif
