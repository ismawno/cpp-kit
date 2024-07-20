#pragma once

#include <concepts>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace kit
{
template <std::floating_point Float> struct transform2D
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
        const builder &parent(const transform2D *parent) const;
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
    const transform2D *parent = nullptr;

    mat3 center_scale_rotate_translate3(bool local = false) const;
    mat3 inverse_center_scale_rotate_translate3(bool local = false) const;

    mat3 inverse_scale_center_rotate_translate3(bool local = false) const;
    mat3 scale_center_rotate_translate3(bool local = false) const;

    mat4 center_scale_rotate_translate4(bool local = false) const;
    mat4 inverse_center_scale_rotate_translate4(bool local = false) const;

    mat4 inverse_scale_center_rotate_translate4(bool local = false) const;
    mat4 scale_center_rotate_translate4(bool local = false) const;

    void ltranslate(const vec2 &dpos);
    void lxtranslate(Float dx);
    void lytranslate(Float dy);

    static trigonometry trigonometric_functions(Float rotation);
    static mat2 rotation_matrix(Float rotation);
    static mat2 inverse_rotation_matrix(Float rotation);

  private:
    static transform2D s_transform;
};

template <std::floating_point Float> struct transform3D
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
    const transform3D *parent = nullptr;

    mat4 center_scale_rotate_translate4(bool local = false) const;
    mat4 inverse_center_scale_rotate_translate4(bool local = false) const;

    mat4 inverse_scale_center_rotate_translate4(bool local = false) const;
    mat4 scale_center_rotate_translate4(bool local = false) const;

    mat3 inverse_rotation() const;

    void ltranslate(const vec3 &dpos);
    void lxtranslate(Float dx);
    void lytranslate(Float dy);
    void lztranslate(Float dz);

    void lrotate(const mat3 &rotmat);
    void grotate(const mat3 &rotmat);

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
