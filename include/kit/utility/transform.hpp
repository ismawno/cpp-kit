#pragma once

#include "kit/interface/serialization.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace kit
{
class transform2D
{
  public:
#ifdef KIT_USE_YAML_CPP
    class serializer : public kit::serializer<transform2D>
    {
      public:
        YAML::Node encode(const transform2D &transform) const override;
        bool decode(const YAML::Node &node, transform2D &transform) const override;
    };
#endif

    struct builder
    {
        builder(const transform2D &transform);
        builder();

        const builder &position(const glm::vec2 &position) const;
        const builder &scale(const glm::vec2 &scale) const;
        const builder &origin(const glm::vec2 &origin) const;
        const builder &rotation(float rotation) const;
        transform2D build() const;
    };

    struct trigonometry
    {
        float c;
        float s;
    };

    glm::vec2 position{0.f};
    glm::vec2 scale{1.f};
    glm::vec2 origin{0.f};
    float rotation = 0.f;

    glm::mat3 center_scale_rotate_translate3() const;
    glm::mat3 inverse_center_scale_rotate_translate3() const;

    glm::mat3 inverse_scale_center_rotate_translate3() const;
    glm::mat3 scale_center_rotate_translate3() const;

    glm::mat4 center_scale_rotate_translate4() const;
    glm::mat4 inverse_center_scale_rotate_translate4() const;

    glm::mat4 inverse_scale_center_rotate_translate4() const;
    glm::mat4 scale_center_rotate_translate4() const;

    void translate_local(const glm::vec2 &dpos);
    void xtranslate_local(float dx);
    void ytranslate_local(float dy);

    static trigonometry trigonometric_functions(float rotation);
    static glm::mat2 rotation_matrix(float rotation);
    static glm::mat2 inverse_rotation_matrix(float rotation);
};

class transform3D
{
  public:
#ifdef KIT_USE_YAML_CPP
    class serializer : public kit::serializer<transform3D>
    {
      public:
        YAML::Node encode(const transform3D &transform) const override;
        bool decode(const YAML::Node &node, transform3D &transform) const override;
    };
#endif

    struct builder
    {
        builder(const transform3D &transform);
        builder();

        const builder &position(const glm::vec3 &position) const;
        const builder &scale(const glm::vec3 &scale) const;
        const builder &origin(const glm::vec3 &origin) const;
        const builder &rotation(const glm::mat3 &rotation) const;
        transform3D build() const;
    };

    struct trigonometry
    {
        float c1;
        float s1;
        float c2;
        float s2;
        float c3;
        float s3;
    };
    struct basis
    {
        glm::vec3 u;
        glm::vec3 v;
        glm::vec3 w;
    };

    glm::vec3 position{0.f};
    glm::vec3 scale{1.f};
    glm::vec3 origin{0.f};
    glm::mat3 rotation{1.f};

    glm::mat4 center_scale_rotate_translate4() const;
    glm::mat4 inverse_center_scale_rotate_translate4() const;

    glm::mat4 inverse_scale_center_rotate_translate4() const;
    glm::mat4 scale_center_rotate_translate4() const;

    glm::mat3 inverse_rotation() const;

    void translate_local(const glm::vec3 &dpos);
    void xtranslate_local(float dx);
    void ytranslate_local(float dy);
    void ztranslate_local(float dz);

    void rotate_local(const glm::mat3 &rotmat);
    void rotate_global(const glm::mat3 &rotmat);

    static trigonometry trigonometric_functions(const glm::vec3 &rotation);

    static glm::mat3 XYZ(const glm::vec3 &rotation);
    static glm::mat3 XZY(const glm::vec3 &rotation);
    static glm::mat3 YXZ(const glm::vec3 &rotation);
    static glm::mat3 YZX(const glm::vec3 &rotation);
    static glm::mat3 ZXY(const glm::vec3 &rotation);
    static glm::mat3 ZYX(const glm::vec3 &rotation);

    static glm::mat3 XY(float rotx, float roty);
    static glm::mat3 XZ(float rotx, float rotz);
    static glm::mat3 YX(float roty, float rotx);
    static glm::mat3 YZ(float roty, float rotz);
    static glm::mat3 ZX(float rotz, float rotx);
    static glm::mat3 ZY(float rotz, float roty);

    static glm::mat3 X(float rotx);
    static glm::mat3 Y(float roty);
    static glm::mat3 Z(float rotz);
};
} // namespace kit
