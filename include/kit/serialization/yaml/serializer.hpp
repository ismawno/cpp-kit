#pragma once
#ifdef KIT_USE_YAML_CPP

#include "kit/serialization/yaml/codec.hpp"
#include "kit/debug/log.hpp"
#include <string>
#include <fstream>

namespace kit::yaml
{
#ifdef KIT_USE_YAML_CPP
template <typename T> void serialize(const T &instance, const std::string &path)
{
    YAML::Emitter out;
    if constexpr (std::is_base_of_v<encodeable, T>)
        out << instance.encode();
    else
        out << codec<T>::encode(instance);

    std::ofstream file(path);
    file << out.c_str();
}

template <typename T> void deserialize(T &instance, const std::string &path)
{
    const YAML::Node node = YAML::LoadFile(path);
    if constexpr (std::is_base_of_v<decodeable, T>)
    {
        KIT_CHECK_RETURN_VALUE(instance.decode(node), true, ERROR,
                               "Failed to deserialize. Attempted to decode with wrong node?")
    }
    else
    {
        KIT_CHECK_RETURN_VALUE(codec<T>::decode(node, instance), true, ERROR,
                               "Failed to deserialize. Attempted to decode with wrong node?")
    }
}

template <typename T> T deserialize(const std::string &path)
{
    T instance;
    deserialize(instance, path);
}
#endif
class serializable : public encodeable
{
#ifdef KIT_USE_YAML_CPP
  public:
    void serialize(const std::string &path) const
    {
        kit::yaml::serialize(*this, path);
    }
#endif
};

class deserializable : public decodeable
{
#ifdef KIT_USE_YAML_CPP
  public:
    void deserialize(const std::string &path)
    {
        kit::yaml::deserialize(*this, path);
    }
#endif
};
} // namespace kit::yaml
#endif