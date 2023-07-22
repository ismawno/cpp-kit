#ifndef KIT_SERIALIZABLE_HPP
#define KIT_SERIALIZABLE_HPP

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
    void deserialize(T &instance, const std::string &path)
    {
        const YAML::Node node = YAML::LoadFile(path);
        KIT_CHECK_RETURN_VALUE(decode(node, instance), true, ERROR,
                               "Failed to deserialize. Attempted to decode with wrong node?")
    }

    virtual YAML::Node encode(const T &) const = 0;
    virtual bool decode(const YAML::Node &node, T &) = 0;
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
YAML::Emitter &operator<<(YAML::Emitter &out, const serializable &srz);
#endif
} // namespace kit

#ifdef KIT_USE_YAML_CPP
namespace YAML
{
template <> struct convert<kit::serializable>
{
    static Node encode(const kit::serializable &srz);
    static bool decode(const Node &node, kit::serializable &srz);
};
} // namespace YAML
#endif

#endif