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
template <FloatingPoint Float> transform2D<Float> transform2D<Float>::builder::build() const
{
    return s_transform;
}

template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::center_scale_rotate_translate3() const
{
    const mat2 scale_matrix = mat2({scale.x, 0.f}, {0.f, scale.y});
    const mat2 linear = rotation_matrix(rotation) * scale_matrix;
    const vec2 translation = position - linear * origin;

    return mat3{glm::vec<3, Float>(linear[0], 0.f), glm::vec<3, Float>(linear[1], 0.f),
                glm::vec<3, Float>(translation, 1.f)};
}
template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::inverse_center_scale_rotate_translate3() const
{
    const mat2 inverse_scale_matrix = mat2({1.f / scale.x, 0.f}, {0.f, 1.f / scale.y});
    const mat2 linear = inverse_scale_matrix * inverse_rotation_matrix(rotation);
    const vec2 translation = origin - linear * position;

    return mat3{glm::vec<3, Float>(linear[0], 0.f), glm::vec<3, Float>(linear[1], 0.f),
                glm::vec<3, Float>(translation, 1.f)};
}

template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::inverse_scale_center_rotate_translate3() const
{
    const mat2 rmat = inverse_rotation_matrix(rotation);
    const vec2 translation = (origin - rmat * position) / scale;

    return mat3{glm::vec<3, Float>(rmat[0] / scale, 0.f), glm::vec<3, Float>(rmat[1] / scale, 0.f),
                glm::vec<3, Float>(translation, 1.f)};
}
template <FloatingPoint Float>
typename transform2D<Float>::mat3 transform2D<Float>::scale_center_rotate_translate3() const
{
    const mat2 rmat = rotation_matrix(rotation);
    const vec2 translation = position - rmat * origin;

    return mat3{glm::vec<3, Float>(rmat[0] * scale.x, 0.f), glm::vec<3, Float>(rmat[1] * scale.y, 0.f),
                glm::vec<3, Float>(translation, 1.f)};
}

template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::center_scale_rotate_translate4() const
{
    const mat2 scale_matrix = mat2({scale.x, 0.f}, {0.f, scale.y});
    const mat2 linear = rotation_matrix(rotation) * scale_matrix;
    const vec2 translation = position - linear * origin;

    return mat4{glm::vec<4, Float>(linear[0], 0.f, 0.f), glm::vec<4, Float>(linear[1], 0.f, 0.f),
                glm::vec<4, Float>(0.f, 0.f, 1.f, 0.f), glm::vec<4, Float>(translation, 0.f, 1.f)};
}
template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::inverse_center_scale_rotate_translate4() const
{
    const mat2 inverse_scale_matrix = mat2({1.f / scale.x, 0.f}, {0.f, 1.f / scale.y});
    const mat2 linear = inverse_scale_matrix * inverse_rotation_matrix(rotation);
    const vec2 translation = origin - linear * position;

    return mat4{glm::vec<4, Float>(linear[0], 0.f, 0.f), glm::vec<4, Float>(linear[1], 0.f, 0.f),
                glm::vec<4, Float>(0.f, 0.f, 1.f, 0.f), glm::vec<4, Float>(translation, 0.f, 1.f)};
}

template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::inverse_scale_center_rotate_translate4() const
{
    const mat2 rmat = inverse_rotation_matrix(rotation);
    const vec2 translation = (origin - rmat * position) / scale;

    return mat4{glm::vec<4, Float>(rmat[0] / scale, 0.f, 0.f), glm::vec<4, Float>(rmat[1] / scale, 0.f, 0.f),
                glm::vec<4, Float>(0.f, 0.f, 1.f, 0.f), glm::vec<4, Float>(translation, 0.f, 1.f)};
}
template <FloatingPoint Float>
typename transform2D<Float>::mat4 transform2D<Float>::scale_center_rotate_translate4() const
{
    const mat2 rmat = rotation_matrix(rotation);
    const vec2 translation = position - rmat * origin;

    return mat4{glm::vec<4, Float>(rmat[0] * scale.x, 0.f, 0.f), glm::vec<4, Float>(rmat[1] * scale.y, 0.f, 0.f),
                glm::vec<4, Float>(0.f, 0.f, 1.f, 0.f), glm::vec<4, Float>(translation, 0.f, 1.f)};
}

template <FloatingPoint Float> void transform2D<Float>::translate_local(const vec2 &dpos)
{
    const auto [c, s] = trigonometric_functions(rotation); // CHANGE THIS MULTIPLY BY ROTMAT
    const vec2 u{c, -s};
    const vec2 v{s, c};
    position += vec2(glm::dot(u, dpos), glm::dot(v, dpos));
}
template <FloatingPoint Float> void transform2D<Float>::xtranslate_local(Float dx)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += vec2(c * dx, s * dx);
}
template <FloatingPoint Float> void transform2D<Float>::ytranslate_local(Float dy)
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
const typename transform3D<Float>::builder &transform3D<Float>::builder::position(
    const glm::vec<3, Float> &position) const
{
    s_transform.position = position;
    return *this;
}
template <FloatingPoint Float>
const typename transform3D<Float>::builder &transform3D<Float>::builder::scale(const glm::vec<3, Float> &scale) const
{
    s_transform.scale = scale;
    return *this;
}
template <FloatingPoint Float>
const typename transform3D<Float>::builder &transform3D<Float>::builder::origin(const glm::vec<3, Float> &origin) const
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
typename transform3D<Float>::mat4 transform3D<Float>::center_scale_rotate_translate4() const
{
    const mat3 scale_matrix = mat3({scale.x, 0.f, 0.f}, {0.f, scale.y, 0.f}, {0.f, 0.f, scale.z});
    const mat3 linear = rotation * scale_matrix;
    const glm::vec<3, Float> translation = position - linear * origin;

    return mat4{glm::vec<4, Float>(linear[0], 0.f), glm::vec<4, Float>(linear[1], 0.f),
                glm::vec<4, Float>(linear[2], 0.f), glm::vec<4, Float>(translation, 1.f)};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::inverse_center_scale_rotate_translate4() const
{
    const mat3 inverse_scale_matrix =
        mat3({1.f / scale.x, 0.f, 0.f}, {0.f, 1.f / scale.y, 0.f}, {0.f, 0.f, 1.f / scale.z});
    const mat3 linear = inverse_scale_matrix * inverse_rotation();
    const glm::vec<3, Float> translation = origin - linear * position;

    return mat4{glm::vec<4, Float>(linear[0], 0.f), glm::vec<4, Float>(linear[1], 0.f),
                glm::vec<4, Float>(linear[2], 0.f), glm::vec<4, Float>(translation, 1.f)};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::inverse_scale_center_rotate_translate4() const
{
    const mat3 rmat = inverse_rotation();
    const glm::vec<3, Float> translation = (origin - rmat * position) / scale;

    return mat4{glm::vec<4, Float>(rmat[0] / scale, 0.f), glm::vec<4, Float>(rmat[1] / scale, 0.f),
                glm::vec<4, Float>(rmat[2] / scale, 0.f), glm::vec<4, Float>(translation, 1.f)};
}
template <FloatingPoint Float>
typename transform3D<Float>::mat4 transform3D<Float>::scale_center_rotate_translate4() const
{
    const glm::vec<3, Float> translation = position - rotation * origin;

    return mat4{glm::vec<4, Float>(rotation[0] * scale.x, 0.f), glm::vec<4, Float>(rotation[1] * scale.y, 0.f),
                glm::vec<4, Float>(rotation[2] * scale.z, 0.f), glm::vec<4, Float>(translation, 1.f)};
}

template <FloatingPoint Float> typename transform3D<Float>::mat3 transform3D<Float>::inverse_rotation() const
{
    return glm::transpose(rotation);
}

template <FloatingPoint Float> void transform3D<Float>::translate_local(const glm::vec<3, Float> &dpos)
{
    position += rotation * dpos;
}
template <FloatingPoint Float> void transform3D<Float>::xtranslate_local(Float dx)
{
    position += rotation[0] * dx;
}
template <FloatingPoint Float> void transform3D<Float>::ytranslate_local(Float dy)
{
    position += rotation[1] * dy;
}
template <FloatingPoint Float> void transform3D<Float>::ztranslate_local(Float dz)
{
    position += rotation[2] * dz;
}

template <FloatingPoint Float> void transform3D<Float>::rotate_local(const mat3 &rotmat)
{
    rotation *= rotmat;
}
template <FloatingPoint Float> void transform3D<Float>::rotate_global(const mat3 &rotmat)
{
    rotation = rotmat * rotation;
}

template <FloatingPoint Float>
typename transform3D<Float>::trigonometry transform3D<Float>::trigonometric_functions(
    const glm::vec<3, Float> &rotation)
{
    return {cos(rotation.x), sin(rotation.x), cos(rotation.y), sin(rotation.y), cos(rotation.z), sin(rotation.z)};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::XYZ(const glm::vec<3, Float> &rotation)
{
    const auto [c1, s1, c2, s2, c3, s3] = trigonometric_functions(rotation);
    return {{(c2 * c3), (c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2)},
            {(-c2 * s3), (c1 * c3 + s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3)},
            {(s2), (-c2 * s1), (c1 * c2)}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::XZY(const glm::vec<3, Float> &rotation)
{
    const auto [c1, s1, c3, s3, c2, s2] = trigonometric_functions(rotation);
    return {{(c2 * c3), (s1 * s3 + c1 * c3 * s2), (c3 * s1 * s2 - c1 * s3)},
            {(-s2), (c1 * c2), (c2 * s1)},
            {(c2 * s3), (c1 * s2 * s3 - c3 * s1), (c1 * c3 + s1 * s2 * s3)}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::YXZ(const glm::vec<3, Float> &rotation)
{
    const auto [c2, s2, c1, s1, c3, s3] = trigonometric_functions(rotation);
    return {{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)},
            {(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)},
            {(c2 * s1), (-s2), (c1 * c2)}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::YZX(const glm::vec<3, Float> &rotation)
{
    const auto [c3, s3, c1, s1, c2, s2] = trigonometric_functions(rotation);
    return {{(c1 * c2), (s2), (-c2 * s1)},
            {(s1 * s3 - c1 * c3 * s2), (c2 * c3), (c1 * s3 + c3 * s1 * s2)},
            {(c3 * s1 + c1 * s2 * s3), (-c2 * s3), (c1 * c3 - s1 * s2 * s3)}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::ZXY(const glm::vec<3, Float> &rotation)
{
    const auto [c2, s2, c3, s3, c1, s1] = trigonometric_functions(rotation);
    return {{(c1 * c3 - s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3), (-c2 * s3)},
            {(-c2 * s1), (c1 * c2), (s2)},
            {(c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2), (c2 * c3)}};
}

template <FloatingPoint Float>
typename transform3D<Float>::mat3 transform3D<Float>::ZYX(const glm::vec<3, Float> &rotation)
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