#include "kit/internal/pch.hpp"
#include "kit/utility/transform.hpp"

namespace kit
{
template <FloatingPoint Float> transform2D<Float> transform2D<Float>::s_transform{};

template <FloatingPoint Float> transform2D<Float>::builder::builder(const transform2D &transform)
{
    s_transform = transform;
}
template <FloatingPoint Float> transform2D<Float>::builder::builder()
{
    s_transform = {};
}
template <FloatingPoint Float>
const typename transform2D<Float>::builder &transform2D<Float>::builder::position(const vec2 &position) const
{
    s_transform.position = position;
    return *this;
}
template <FloatingPoint Float>
const typename transform2D<Float>::builder &transform2D<Float>::builder::scale(const vec2 &scale) const
{
    s_transform.scale = scale;
    return *this;
}
template <FloatingPoint Float>
const typename transform2D<Float>::builder &transform2D<Float>::builder::origin(const vec2 &origin) const
{
    s_transform.origin = origin;
    return *this;
}
template <FloatingPoint Float>
const typename transform2D<Float>::builder &transform2D<Float>::builder::rotation(const Float rotation) const
{
    s_transform.rotation = rotation;
    return *this;
}
template <FloatingPoint Float>
const typename transform2D<Float>::builder &transform2D<Float>::builder::parent(const transform2D *parent) const
{
    s_transform.parent = parent;
    return *this;
}
template <FloatingPoint Float> transform2D<Float> transform2D<Float>::builder::build() const
{
    return s_transform;
}

template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::center_scale_rotate_translate3(const bool local) const
{
    const mat2 scale_matrix = mat2({scale.x, 0.f}, {0.f, scale.y});
    const mat2 linear = rotation_matrix(rotation) * scale_matrix;
    const vec2 translation = position - linear * origin;

    const mat3 result = mat3{vec3(linear[0], 0.f), vec3(linear[1], 0.f), vec3(translation, 1.f)};
    return parent && !local ? parent->center_scale_rotate_translate3() * result : result;
}
template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::inverse_center_scale_rotate_translate3(const bool local) const
{
    const mat2 inverse_scale_matrix = mat2({1.f / scale.x, 0.f}, {0.f, 1.f / scale.y});
    const mat2 linear = inverse_scale_matrix * inverse_rotation_matrix(rotation);
    const vec2 translation = origin - linear * position;

    const mat3 result = mat3{vec3(linear[0], 0.f), vec3(linear[1], 0.f), vec3(translation, 1.f)};
    return parent && !local ? result * parent->inverse_center_scale_rotate_translate3() : result;
}

template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::inverse_scale_center_rotate_translate3(const bool local) const
{
    const mat2 rmat = inverse_rotation_matrix(rotation);
    const vec2 translation = (origin - rmat * position) / scale;

    const mat3 result = mat3{vec3(rmat[0] / scale, 0.f), vec3(rmat[1] / scale, 0.f), vec3(translation, 1.f)};
    return parent && !local ? result * parent->inverse_scale_center_rotate_translate3() : result;
}
template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::scale_center_rotate_translate3(const bool local) const
{
    const mat2 rmat = rotation_matrix(rotation);
    const vec2 translation = position - rmat * origin;

    const mat3 result = mat3{vec3(rmat[0] * scale.x, 0.f), vec3(rmat[1] * scale.y, 0.f), vec3(translation, 1.f)};
    return parent && !local ? parent->scale_center_rotate_translate3() * result : result;
}

template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::center_scale_rotate_translate4(const bool local) const
{
    const mat2 scale_matrix = mat2({scale.x, 0.f}, {0.f, scale.y});
    const mat2 linear = rotation_matrix(rotation) * scale_matrix;
    const vec2 translation = position - linear * origin;

    const mat4 result = mat4{vec4(linear[0], 0.f, 0.f), vec4(linear[1], 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f),
                             vec4(translation, 0.f, 1.f)};
    return parent && !local ? parent->center_scale_rotate_translate4() * result : result;
}
template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::inverse_center_scale_rotate_translate4(const bool local) const
{
    const mat2 inverse_scale_matrix = mat2({1.f / scale.x, 0.f}, {0.f, 1.f / scale.y});
    const mat2 linear = inverse_scale_matrix * inverse_rotation_matrix(rotation);
    const vec2 translation = origin - linear * position;

    const mat4 result = mat4{vec4(linear[0], 0.f, 0.f), vec4(linear[1], 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f),
                             vec4(translation, 0.f, 1.f)};
    return parent && !local ? result * parent->inverse_center_scale_rotate_translate4() : result;
}

template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::inverse_scale_center_rotate_translate4(const bool local) const
{
    const mat2 rmat = inverse_rotation_matrix(rotation);
    const vec2 translation = (origin - rmat * position) / scale;

    const mat4 result = mat4{vec4(rmat[0] / scale, 0.f, 0.f), vec4(rmat[1] / scale, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f),
                             vec4(translation, 0.f, 1.f)};
    return parent && !local ? result * parent->inverse_scale_center_rotate_translate4() : result;
}
template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::scale_center_rotate_translate4(const bool local) const
{
    const mat2 rmat = rotation_matrix(rotation);
    const vec2 translation = position - rmat * origin;

    const mat4 result = mat4{vec4(rmat[0] * scale.x, 0.f, 0.f), vec4(rmat[1] * scale.y, 0.f, 0.f),
                             vec4(0.f, 0.f, 1.f, 0.f), vec4(translation, 0.f, 1.f)};
    return parent && !local ? parent->scale_center_rotate_translate4() * result : result;
}

template <FloatingPoint Float> void transform2D<Float>::ltranslate(const vec2 &dpos)
{
    const auto [c, s] = trigonometric_functions(rotation); // CHANGE THIS MULTIPLY BY ROTMAT
    const vec2 u{c, -s};
    const vec2 v{s, c};
    position += vec2(glm::dot(u, dpos), glm::dot(v, dpos));
}
template <FloatingPoint Float> void transform2D<Float>::lxtranslate(const Float dx)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += vec2(c * dx, s * dx);
}
template <FloatingPoint Float> void transform2D<Float>::lytranslate(const Float dy)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += vec2(-s * dy, c * dy);
}

template <FloatingPoint Float>
typename transform2D<Float>::trigonometry transform2D<Float>::trigonometric_functions(const Float rotation)
{
    return {cos(rotation), sin(rotation)};
}

template <FloatingPoint Float>
typename transform2D<Float>::mat2 transform2D<Float>::rotation_matrix(const Float rotation)
{
    const auto [c, s] = trigonometric_functions(rotation);
    return {{c, s}, {-s, c}};
}
template <FloatingPoint Float>
typename transform2D<Float>::mat2 transform2D<Float>::inverse_rotation_matrix(const Float rotation)
{
    return rotation_matrix(-rotation);
}

template <FloatingPoint Float> transform3D<Float> transform3D<Float>::s_transform{};

template <FloatingPoint Float> transform3D<Float>::builder::builder(const transform3D &transform)
{
    s_transform = transform;
}
template <FloatingPoint Float> transform3D<Float>::builder::builder()
{
    s_transform = {};
}
template <FloatingPoint Float>
const typename transform3D<Float>::builder &transform3D<Float>::builder::position(const vec3 &position) const
{
    s_transform.position = position;
    return *this;
}
template <FloatingPoint Float>
const typename transform3D<Float>::builder &transform3D<Float>::builder::scale(const vec3 &scale) const
{
    s_transform.scale = scale;
    return *this;
}
template <FloatingPoint Float>
const typename transform3D<Float>::builder &transform3D<Float>::builder::origin(const vec3 &origin) const
{
    s_transform.origin = origin;
    return *this;
}
template <FloatingPoint Float>
const typename transform3D<Float>::builder &transform3D<Float>::builder::rotation(const mat3 &rotation) const
{
    s_transform.rotation = rotation;
    return *this;
}
template <FloatingPoint Float> transform3D<Float> transform3D<Float>::builder::build() const
{
    return s_transform;
}

template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::center_scale_rotate_translate4(const bool local) const
{
    const mat3 scale_matrix = mat3({scale.x, 0.f, 0.f}, {0.f, scale.y, 0.f}, {0.f, 0.f, scale.z});
    const mat3 linear = rotation * scale_matrix;
    const vec3 translation = position - linear * origin;

    const mat4 result = mat4{vec4(linear[0], 0.f), vec4(linear[1], 0.f), vec4(linear[2], 0.f), vec4(translation, 1.f)};
    return parent && !local ? parent->center_scale_rotate_translate4() * result : result;
}

template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::inverse_center_scale_rotate_translate4(const bool local) const
{
    const mat3 inverse_scale_matrix =
        mat3({1.f / scale.x, 0.f, 0.f}, {0.f, 1.f / scale.y, 0.f}, {0.f, 0.f, 1.f / scale.z});
    const mat3 linear = inverse_scale_matrix * inverse_rotation();
    const vec3 translation = origin - linear * position;

    const mat4 result = mat4{vec4(linear[0], 0.f), vec4(linear[1], 0.f), vec4(linear[2], 0.f), vec4(translation, 1.f)};
    return parent && !local ? result * parent->inverse_center_scale_rotate_translate4() : result;
}

template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::inverse_scale_center_rotate_translate4(const bool local) const
{
    const mat3 rmat = inverse_rotation();
    const vec3 translation = (origin - rmat * position) / scale;

    const mat4 result = mat4{vec4(rmat[0] / scale, 0.f), vec4(rmat[1] / scale, 0.f), vec4(rmat[2] / scale, 0.f),
                             vec4(translation, 1.f)};
    return parent && !local ? result * parent->inverse_scale_center_rotate_translate4() : result;
}
template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::scale_center_rotate_translate4(const bool local) const
{
    const vec3 translation = position - rotation * origin;
    const mat4 result = mat4{vec4(rotation[0] * scale.x, 0.f), vec4(rotation[1] * scale.y, 0.f),
                             vec4(rotation[2] * scale.z, 0.f), vec4(translation, 1.f)};
    return parent && !local ? parent->scale_center_rotate_translate4() * result : result;
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::inverse_rotation() const
{
    return glm::transpose(rotation);
}

template <FloatingPoint Float> void transform3D<Float>::ltranslate(const vec3 &dpos)
{
    position += rotation * dpos;
}
template <FloatingPoint Float> void transform3D<Float>::lxtranslate(Float dx)
{
    position += rotation[0] * dx;
}
template <FloatingPoint Float> void transform3D<Float>::lytranslate(Float dy)
{
    position += rotation[1] * dy;
}
template <FloatingPoint Float> void transform3D<Float>::lztranslate(Float dz)
{
    position += rotation[2] * dz;
}

template <FloatingPoint Float> void transform3D<Float>::lrotate(const mat3 &rotmat)
{
    rotation *= rotmat;
}
template <FloatingPoint Float> void transform3D<Float>::grotate(const mat3 &rotmat)
{
    rotation = rotmat * rotation;
}

template <FloatingPoint Float>
typename transform3D<Float>::trigonometry transform3D<Float>::trigonometric_functions(const vec3 &rotation)
{
    return {cos(rotation.x), sin(rotation.x), cos(rotation.y), sin(rotation.y), cos(rotation.z), sin(rotation.z)};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::XYZ(const vec3 &rotation)
{
    const auto [c1, s1, c2, s2, c3, s3] = trigonometric_functions(rotation);
    return {{(c2 * c3), (c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2)},
            {(-c2 * s3), (c1 * c3 + s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3)},
            {(s2), (-c2 * s1), (c1 * c2)}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::XZY(const vec3 &rotation)
{
    const auto [c1, s1, c3, s3, c2, s2] = trigonometric_functions(rotation);
    return {{(c2 * c3), (s1 * s3 + c1 * c3 * s2), (c3 * s1 * s2 - c1 * s3)},
            {(-s2), (c1 * c2), (c2 * s1)},
            {(c2 * s3), (c1 * s2 * s3 - c3 * s1), (c1 * c3 + s1 * s2 * s3)}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::YXZ(const vec3 &rotation)
{
    const auto [c2, s2, c1, s1, c3, s3] = trigonometric_functions(rotation);
    return {{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)},
            {(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)},
            {(c2 * s1), (-s2), (c1 * c2)}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::YZX(const vec3 &rotation)
{
    const auto [c3, s3, c1, s1, c2, s2] = trigonometric_functions(rotation);
    return {{(c1 * c2), (s2), (-c2 * s1)},
            {(s1 * s3 - c1 * c3 * s2), (c2 * c3), (c1 * s3 + c3 * s1 * s2)},
            {(c3 * s1 + c1 * s2 * s3), (-c2 * s3), (c1 * c3 - s1 * s2 * s3)}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::ZXY(const vec3 &rotation)
{
    const auto [c2, s2, c3, s3, c1, s1] = trigonometric_functions(rotation);
    return {{(c1 * c3 - s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3), (-c2 * s3)},
            {(-c2 * s1), (c1 * c2), (s2)},
            {(c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2), (c2 * c3)}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::ZYX(const vec3 &rotation)
{
    const auto [c3, s3, c2, s2, c1, s1] = trigonometric_functions(rotation);
    return {{(c1 * c2), (c2 * s1), (-s2)},
            {(c1 * s2 * s3 - c3 * s1), (c1 * c3 + s1 * s2 * s3), (c2 * s3)},
            {(s1 * s3 + c1 * c3 * s2), (c3 * s1 * s2 - c1 * s3), (c2 * c3)}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::XY(const Float rotx, const Float roty)
{
    const auto [c1, s1] = transform2D<Float>::trigonometric_functions(rotx);
    const auto [c2, s2] = transform2D<Float>::trigonometric_functions(roty);
    return {{c2, s1 * s2, -c1 * s2}, {0.f, c1, s1}, {s2, -c2 * s1, c1 * c2}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::XZ(const Float rotx, const Float rotz)
{
    const auto [c1, s1] = transform2D<Float>::trigonometric_functions(rotx);
    const auto [c3, s3] = transform2D<Float>::trigonometric_functions(rotz);
    return {{c3, c1 * s3, s1 * s3}, {-s3, c1 * c3, c3 * s1}, {0.f, -s1, c1}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::YX(const Float roty, const Float rotx)
{
    const auto [c1, s1] = transform2D<Float>::trigonometric_functions(rotx);
    const auto [c2, s2] = transform2D<Float>::trigonometric_functions(roty);
    return {{c2, 0.f, -s2}, {s1 * s2, c1, c2 * s1}, {c1 * s2, -s1, c1 * c2}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::YZ(Float roty, Float rotz)
{
    const auto [c2, s2] = transform2D<Float>::trigonometric_functions(roty);
    const auto [c3, s3] = transform2D<Float>::trigonometric_functions(rotz);
    return {{c2 * c3, s3, -c3 * s2}, {-c2 * s3, c3, s2 * s3}, {s2, 0.f, c2}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::ZX(const Float rotz, const Float rotx)
{
    const auto [c1, s1] = transform2D<Float>::trigonometric_functions(rotx);
    const auto [c3, s3] = transform2D<Float>::trigonometric_functions(rotz);
    return {{c3, s3, 0.f}, {-c1 * s3, c1 * c3, s1}, {s1 * s3, -c3 * s1, c1}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::ZY(Float rotz, Float roty)
{
    const auto [c2, s2] = transform2D<Float>::trigonometric_functions(roty);
    const auto [c3, s3] = transform2D<Float>::trigonometric_functions(rotz);
    return {{c2 * c3, c2 * s3, -s2}, {-s3, c3, 0.f}, {c3 * s2, s2 * s3, c2}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::X(const Float rotx)
{
    const auto [c, s] = transform2D<Float>::trigonometric_functions(rotx);
    return {{1.f, 0.f, 0.f}, {0.f, c, s}, {0.f, -s, c}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::Y(const Float roty)
{
    const auto [c, s] = transform2D<Float>::trigonometric_functions(roty);
    return {{c, 0.f, -s}, {0.f, 1.f, 0.f}, {s, 0.f, c}};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::Z(const Float rotz)
{
    const auto [c, s] = transform2D<Float>::trigonometric_functions(rotz);
    return {{c, s, 0.f}, {-s, c, 0.f}, {0.f, 0.f, 1.f}};
}

template struct transform2D<float>;
template struct transform2D<double>;

template struct transform3D<float>;
template struct transform3D<double>;

} // namespace kit