#ifdef KIT_USE_YAML_CPP
#include "kit/internal/pch.hpp"
#include "kit/interface/serialization.hpp"

namespace kit
{
void serializable::serialize(const std::string &path) const
{
    const YAML::Node node = encode();
    YAML::Emitter out;
    out << node;

    std::ofstream file(path);
    file << out.c_str();
}
void serializable::deserialize(const std::string &path)
{
    const YAML::Node node = YAML::LoadFile(path);
    KIT_CHECK_RETURN_VALUE(decode(node), true, ERROR, "Failed to deserialize. Attempted to decode with wrong node?")
}
} // namespace kit

#endif