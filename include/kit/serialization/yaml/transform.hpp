#pragma once
#ifdef KIT_USE_YAML_CPP

#include "kit/serialization/yaml/codec.hpp"
#include "kit/serialization/yaml/glm.hpp"
#include "kit/utility/transform.hpp"

template <std::floating_point Float> struct kit::yaml::codec<kit::transform2D<Float>>
{
    static YAML::Node encode(const kit::transform2D<Float> &transform)
    {
        YAML::Node node;
        node["Position"] = transform.position;
        node["Scale"] = transform.scale;
        node["Origin"] = transform.origin;
        node["Rotation"] = transform.rotation;
        return node;
    }

    static bool decode(const YAML::Node &node, kit::transform2D<Float> &transform)
    {
        if (!node.IsMap() || node.size() != 4)
            return false;
        transform.position = node["Position"].as<glm::vec<2, Float>>();
        transform.scale = node["Scale"].as<glm::vec<2, Float>>();
        transform.origin = node["Origin"].as<glm::vec<2, Float>>();
        transform.rotation = node["Rotation"].as<float>();
        return true;
    }
};

template <std::floating_point Float> struct kit::yaml::codec<kit::transform3D<Float>>
{
    static YAML::Node encode(const kit::transform3D<Float> &transform)
    {
        YAML::Node node;
        node["Position"] = transform.position;
        node["Scale"] = transform.scale;
        node["Origin"] = transform.origin;
        node["XRotation"] = transform.rotation[0];
        node["YRotation"] = transform.rotation[1];
        node["ZRotation"] = transform.rotation[2];
        return node;
    }

    static bool decode(const YAML::Node &node, kit::transform3D<Float> &transform)
    {
        if (!node.IsMap() || node.size() != 6)
            return false;
        transform.position = node["Position"].as<glm::vec<3, Float>>();
        transform.scale = node["Scale"].as<glm::vec<3, Float>>();
        transform.origin = node["Origin"].as<glm::vec<3, Float>>();
        transform.rotation[0] = node["XRotation"].as<glm::vec<3, Float>>();
        transform.rotation[1] = node["YRotation"].as<glm::vec<3, Float>>();
        transform.rotation[2] = node["ZRotation"].as<glm::vec<3, Float>>();
        return true;
    }
};
#endif