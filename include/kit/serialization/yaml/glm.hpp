#pragma once
#ifdef KIT_USE_YAML_CPP
#include "kit/serialization/yaml/codec.hpp"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

template <typename Float> struct kit::yaml::codec<glm::vec<2, Float>>
{
    static YAML::Node encode(const glm::vec<2, Float> &v)
    {
        YAML::Node node;
        node.push_back(v.x);
        node.push_back(v.y);
        node.SetStyle(YAML::EmitterStyle::Flow);
        return node;
    }

    static bool decode(const YAML::Node &node, glm::vec<2, Float> &v)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        v.x = node[0].as<Float>();
        v.y = node[1].as<Float>();
        return true;
    }
};

template <typename Float> struct kit::yaml::codec<glm::vec<3, Float>>
{
    static YAML::Node encode(const glm::vec<3, Float> &v)
    {
        YAML::Node node;
        node.push_back(v.x);
        node.push_back(v.y);
        node.push_back(v.z);
        node.SetStyle(YAML::EmitterStyle::Flow);
        return node;
    }

    static bool decode(const YAML::Node &node, glm::vec<3, Float> &v)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        v.x = node[0].as<Float>();
        v.y = node[1].as<Float>();
        v.z = node[2].as<Float>();
        return true;
    }
};

template <typename Float> struct kit::yaml::codec<glm::vec<4, Float>>
{
    static YAML::Node encode(const glm::vec<4, Float> &v)
    {
        YAML::Node node;
        node.push_back(v.x);
        node.push_back(v.y);
        node.push_back(v.z);
        node.push_back(v.w);
        node.SetStyle(YAML::EmitterStyle::Flow);
        return node;
    }

    static bool decode(const YAML::Node &node, glm::vec<4, Float> &v)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        v.x = node[0].as<Float>();
        v.y = node[1].as<Float>();
        v.z = node[2].as<Float>();
        v.w = node[3].as<Float>();
        return true;
    }
};
#endif