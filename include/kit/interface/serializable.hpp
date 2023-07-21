#ifndef KIT_SERIALIZABLE_HPP
#define KIT_SERIALIZABLE_HPP

#ifdef KIT_USE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

#include <string>

namespace kit
{
class serializable
{
#ifdef KIT_USE_YAML_CPP
  public:
    void serialize(const std::string &path) const;
    void deserialize(const std::string &path);

  private:
    virtual YAML::Node encode() const = 0;
    virtual bool decode(const YAML::Node &node) = 0;

#ifdef KIT_USE_YAML_CPP
    friend YAML::Emitter &operator<<(YAML::Emitter &, const serializable &);
    friend struct YAML::convert<serializable>;
#endif
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