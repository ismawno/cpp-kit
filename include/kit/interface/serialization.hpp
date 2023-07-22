#ifndef KIT_SERIALIZATION_HPP
#define KIT_SERIALIZATION_HPP

#ifdef KIT_USE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

#include <string>

namespace kit
{
template <typename T> class serializer
{
#ifdef KIT_USE_YAML_CPP
  public:
    void serialize(const T &instance, const std::string &path) const
    {
        const YAML::Node node = encode(instance);
        YAML::Emitter out;
        out << node;

        std::ofstream file(path);
        file << out.c_str();
    }
    void deserialize(T &instance, const std::string &path) const
    {
        const YAML::Node node = YAML::LoadFile(path);
        KIT_CHECK_RETURN_VALUE(decode(node, instance), true, ERROR,
                               "Failed to deserialize. Attempted to decode with wrong node?")
    }
    T deserialize(const std::string &path) const
    {
        T instance;
        deserialize(instance, path);
        return instance;
    }

    virtual YAML::Node encode(const T &) const = 0;
    virtual bool decode(const YAML::Node &node, T &) const = 0;
#endif
};

class serializable
{
#ifdef KIT_USE_YAML_CPP
  public:
    void serialize(const std::string &path) const;
    void deserialize(const std::string &path);

    virtual YAML::Node encode() const = 0;
    virtual bool decode(const YAML::Node &node) = 0;
#endif
};

#ifdef KIT_USE_YAML_CPP
template <typename T> YAML::Emitter &operator<<(YAML::Emitter &out, const T &instance)
{
    out << YAML::BeginMap;
    if constexpr (std::is_base_of<serializable, T>::value)
        out << instance.encode();
    else
    {
        static_assert(std::is_base_of<kit::serializer<T>, typename T::serializer>::value,
                      "Nested serializer class of type T must inherit from serializer<T>");
        const typename T::serializer srz;
        out << srz.encode(instance);
    }
    out << YAML::EndMap;
    return out;
}
#endif
} // namespace kit

#ifdef KIT_USE_YAML_CPP
namespace YAML
{
template <typename T> struct convert
{
    static Node encode(const T &instance)
    {
        if constexpr (std::is_base_of<kit::serializable, T>::value)
            return instance.encode();
        else
        {
            const typename T::serializer srz;
            return srz.encode(instance);
        }
    }
    static bool decode(const Node &node, T &instance)
    {
        if constexpr (std::is_base_of<kit::serializable, T>::value)
            return instance.decode(node);
        else
        {
            static_assert(std::is_base_of<kit::serializer<T>, typename T::serializer>::value,
                          "Nested serializer class of type T must inherit from serializer<T>");
            const typename T::serializer srz;
            return srz.decode(node, instance);
        }
    }
};
} // namespace YAML
#endif

#endif