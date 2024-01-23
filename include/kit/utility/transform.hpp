#pragma once

#include "kit/utility/type_constraints.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace kit
{
template <FloatingPoint Float> struct transform2D
{
    using vec2 = glm::vec<2, Float>;
    using vec3 = glm::vec<3, Float>;
    using vec4 = glm::vec<4, Float>;
    using mat2 = glm::mat<2, 2, Float>;
    using mat3 = glm::mat<3, 3, Float>;
    using mat4 = glm::mat<4, 4, Float>;

    struct builder
    {
        builder(const transform2D &transform);
        builder();

        const builder &position(const vec2 &position) const;
        const builder &scale(const vec2 &scale) const;
        const builder &origin(const vec2 &origin) const;
        const builder &rotation(Float rotation) const;
        transform2D build() const;
    };

    struct trigonometry
    {
        Float c;
        Float s;
    };

    vec2 position{0.f};
    vec2 scale{1.f};
    vec2 origin{0.f};
    Float rotation = 0.f;
    transform2D *parent = nullptr;

    mat3 center_scale_rotate_translate3() const;
    mat3 inverse_center_scale_rotate_translate3() const;

    mat3 inverse_scale_center_rotate_translate3() const;
    mat3 scale_center_rotate_translate3() const;

    mat4 center_scale_rotate_translate4() const;
    mat4 inverse_center_scale_rotate_translate4() const;

    mat4 inverse_scale_center_rotate_translate4() const;
    mat4 scale_center_rotate_translate4() const;

    void translate_local(const vec2 &dpos);
    void xtranslate_local(Float dx);
    void ytranslate_local(Float dy);

    static trigonometry trigonometric_functions(Float rotation);
    static mat2 rotation_matrix(Float rotation);
    static mat2 inverse_rotation_matrix(Float rotation);

  private:
    static transform2D s_transform;
};

template <FloatingPoint Float> struct transform3D
{
    using vec3 = glm::vec<3, Float>;
    using vec4 = glm::vec<4, Float>;
    using mat3 = glm::mat<3, 3, Float>;
    using mat4 = glm::mat<4, 4, Float>;

    struct builder
    {
        builder(const transform3D &transform);
        builder();

        const builder &position(const vec3 &position) const;
        const builder &scale(const vec3 &scale) const;
        const builder &origin(const vec3 &origin) const;
        const builder &rotation(const mat3 &rotation) const;
        transform3D build() const;
    };

    struct trigonometry
    {
        Float c1;
        Float s1;
        Float c2;
        Float s2;
        Float c3;
        Float s3;
    };
    struct basis
    {
        vec3 u;
        vec3 v;
        vec3 w;
    };

    vec3 position{0.f};
    vec3 scale{1.f};
    vec3 origin{0.f};
    mat3 rotation{1.f};
    transform3D *parent = nullptr;

    mat4 center_scale_rotate_translate4() const;
    mat4 inverse_center_scale_rotate_translate4() const;

    mat4 inverse_scale_center_rotate_translate4() const;
    mat4 scale_center_rotate_translate4() const;

    mat3 inverse_rotation() const;

    void translate_local(const vec3 &dpos);
    void xtranslate_local(Float dx);
    void ytranslate_local(Float dy);
    void ztranslate_local(Float dz);

    void rotate_local(const mat3 &rotmat);
    void rotate_global(const mat3 &rotmat);

    static trigonometry trigonometric_functions(const vec3 &rotation);

    static mat3 XYZ(const vec3 &rotation);
    static mat3 XZY(const vec3 &rotation);
    static mat3 YXZ(const vec3 &rotation);
    static mat3 YZX(const vec3 &rotation);
    static mat3 ZXY(const vec3 &rotation);
    static mat3 ZYX(const vec3 &rotation);

    static mat3 XY(Float rotx, Float roty);
    static mat3 XZ(Float rotx, Float rotz);
    static mat3 YX(Float roty, Float rotx);
    static mat3 YZ(Float roty, Float rotz);
    static mat3 ZX(Float rotz, Float rotx);
    static mat3 ZY(Float rotz, Float roty);

    static mat3 X(Float rotx);
    static mat3 Y(Float roty);
    static mat3 Z(Float rotz);

  private:
    static transform3D s_transform;
};
} // namespace kit
