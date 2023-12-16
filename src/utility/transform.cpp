#include "kit/internal/pch.hpp"
#include "kit/utility/transform.hpp"

namespace kit
{
static thread_local transform2D s_transform2;

transform2D::builder::builder(const transform2D &transform)
{
    s_transform2 = transform;
}
transform2D::builder::builder()
{
    s_transform2 = {};
}
const transform2D::builder &transform2D::builder::position(const glm::vec2 &position) const
{
    s_transform2.position = position;
    return *this;
}
const transform2D::builder &transform2D::builder::scale(const glm::vec2 &scale) const
{
    s_transform2.scale = scale;
    return *this;
}
const transform2D::builder &transform2D::builder::origin(const glm::vec2 &origin) const
{
    s_transform2.origin = origin;
    return *this;
}
const transform2D::builder &transform2D::builder::rotation(const float rotation) const
{
    s_transform2.rotation = rotation;
    return *this;
}
transform2D transform2D::builder::build() const
{
    return s_transform2;
}

glm::mat3 transform2D::center_scale_rotate_translate3() const
{
    const glm::mat2 scale_matrix = glm::mat2({scale.x, 0.f}, {0.f, scale.y});
    const glm::mat2 linear = rotation_matrix(rotation) * scale_matrix;
    const glm::vec2 translation = position - linear * origin;

    return glm::mat3{glm::vec3(linear[0], 0.f), glm::vec3(linear[1], 0.f), glm::vec3(translation, 1.f)};
}
glm::mat3 transform2D::inverse_center_scale_rotate_translate3() const
{
    const glm::mat2 inverse_scale_matrix = glm::mat2({1.f / scale.x, 0.f}, {0.f, 1.f / scale.y});
    const glm::mat2 linear = inverse_scale_matrix * inverse_rotation_matrix(rotation);
    const glm::vec2 translation = origin - linear * position;

    return glm::mat3{glm::vec3(linear[0], 0.f), glm::vec3(linear[1], 0.f), glm::vec3(translation, 1.f)};
}

glm::mat3 transform2D::inverse_scale_center_rotate_translate3() const
{
    const glm::mat2 rmat = inverse_rotation_matrix(rotation);
    const glm::vec2 translation = (origin - rmat * position) / scale;

    return glm::mat3{glm::vec3(rmat[0] / scale, 0.f), glm::vec3(rmat[1] / scale, 0.f), glm::vec3(translation, 1.f)};
}
glm::mat3 transform2D::scale_center_rotate_translate3() const
{
    const glm::mat2 rmat = rotation_matrix(rotation);
    const glm::vec2 translation = position - rmat * origin;

    return glm::mat3{glm::vec3(rmat[0] * scale.x, 0.f), glm::vec3(rmat[1] * scale.y, 0.f), glm::vec3(translation, 1.f)};
}

glm::mat4 transform2D::center_scale_rotate_translate4() const
{
    const glm::mat2 scale_matrix = glm::mat2({scale.x, 0.f}, {0.f, scale.y});
    const glm::mat2 linear = rotation_matrix(rotation) * scale_matrix;
    const glm::vec2 translation = position - linear * origin;

    return glm::mat4{glm::vec4(linear[0], 0.f, 0.f), glm::vec4(linear[1], 0.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 0.f),
                     glm::vec4(translation, 0.f, 1.f)};
}
glm::mat4 transform2D::inverse_center_scale_rotate_translate4() const
{
    const glm::mat2 inverse_scale_matrix = glm::mat2({1.f / scale.x, 0.f}, {0.f, 1.f / scale.y});
    const glm::mat2 linear = inverse_scale_matrix * inverse_rotation_matrix(rotation);
    const glm::vec2 translation = origin - linear * position;

    return glm::mat4{glm::vec4(linear[0], 0.f, 0.f), glm::vec4(linear[1], 0.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 0.f),
                     glm::vec4(translation, 0.f, 1.f)};
}

glm::mat4 transform2D::inverse_scale_center_rotate_translate4() const
{
    const glm::mat2 rmat = inverse_rotation_matrix(rotation);
    const glm::vec2 translation = (origin - rmat * position) / scale;

    return glm::mat4{glm::vec4(rmat[0] / scale, 0.f, 0.f), glm::vec4(rmat[1] / scale, 0.f, 0.f),
                     glm::vec4(0.f, 0.f, 1.f, 0.f), glm::vec4(translation, 0.f, 1.f)};
}
glm::mat4 transform2D::scale_center_rotate_translate4() const
{
    const glm::mat2 rmat = rotation_matrix(rotation);
    const glm::vec2 translation = position - rmat * origin;

    return glm::mat4{glm::vec4(rmat[0] * scale.x, 0.f, 0.f), glm::vec4(rmat[1] * scale.y, 0.f, 0.f),
                     glm::vec4(0.f, 0.f, 1.f, 0.f), glm::vec4(translation, 0.f, 1.f)};
}

void transform2D::translate_local(const glm::vec2 &dpos)
{
    const auto [c, s] = trigonometric_functions(rotation); // CHANGE THIS MULTIPLY BY ROTMAT
    const glm::vec2 u{c, -s};
    const glm::vec2 v{s, c};
    position += glm::vec2(glm::dot(u, dpos), glm::dot(v, dpos));
}
void transform2D::xtranslate_local(float dx)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += glm::vec2(c * dx, s * dx);
}
void transform2D::ytranslate_local(float dy)
{
    const auto [c, s] = trigonometric_functions(rotation);
    position += glm::vec2(-s * dy, c * dy);
}

transform2D::trigonometry transform2D::trigonometric_functions(const float rotation)
{
    return {cosf(rotation), sinf(rotation)};
}

glm::mat2 transform2D::rotation_matrix(const float rotation)
{
    const auto [c, s] = trigonometric_functions(rotation);
    return {{c, s}, {-s, c}};
}
glm::mat2 transform2D::inverse_rotation_matrix(const float rotation)
{
    return rotation_matrix(-rotation);
}

#ifdef KIT_USE_YAML_CPP
YAML::Node transform2D::serializer::encode(const transform2D &transform) const
{
    YAML::Node node;
    node["Position"] = transform.position;
    node["Scale"] = transform.scale;
    node["Origin"] = transform.origin;
    node["Rotation"] = transform.rotation;
    return node;
}
bool transform2D::serializer::decode(const YAML::Node &node, transform2D &transform) const
{
    if (!node.IsMap() || node.size() != 4)
        return false;
    transform.position = node["Position"].as<glm::vec2>();
    transform.scale = node["Scale"].as<glm::vec2>();
    transform.origin = node["Origin"].as<glm::vec2>();
    transform.rotation = node["Rotation"].as<float>();
    return true;
}
#endif

static thread_local transform3D s_transform3;

transform3D::builder::builder(const transform3D &transform)
{
    s_transform3 = transform;
}
transform3D::builder::builder()
{
    s_transform3 = {};
}
const transform3D::builder &transform3D::builder::position(const glm::vec3 &position) const
{
    s_transform3.position = position;
    return *this;
}
const transform3D::builder &transform3D::builder::scale(const glm::vec3 &scale) const
{
    s_transform3.scale = scale;
    return *this;
}
const transform3D::builder &transform3D::builder::origin(const glm::vec3 &origin) const
{
    s_transform3.origin = origin;
    return *this;
}
const transform3D::builder &transform3D::builder::rotation(const glm::mat3 &rotation) const
{
    s_transform3.rotation = rotation;
    return *this;
}
transform3D transform3D::builder::build() const
{
    return s_transform3;
}

glm::mat4 transform3D::center_scale_rotate_translate4() const
{
    const glm::mat3 scale_matrix = glm::mat3({scale.x, 0.f, 0.f}, {0.f, scale.y, 0.f}, {0.f, 0.f, scale.z});
    const glm::mat3 linear = rotation * scale_matrix;
    const glm::vec3 translation = position - linear * origin;

    return glm::mat4{glm::vec4(linear[0], 0.f), glm::vec4(linear[1], 0.f), glm::vec4(linear[2], 0.f),
                     glm::vec4(translation, 1.f)};
}

glm::mat4 transform3D::inverse_center_scale_rotate_translate4() const
{
    const glm::mat3 inverse_scale_matrix =
        glm::mat3({1.f / scale.x, 0.f, 0.f}, {0.f, 1.f / scale.y, 0.f}, {0.f, 0.f, 1.f / scale.z});
    const glm::mat3 linear = inverse_scale_matrix * inverse_rotation();
    const glm::vec3 translation = origin - linear * position;

    return glm::mat4{glm::vec4(linear[0], 0.f), glm::vec4(linear[1], 0.f), glm::vec4(linear[2], 0.f),
                     glm::vec4(translation, 1.f)};
}

glm::mat4 transform3D::inverse_scale_center_rotate_translate4() const
{
    const glm::mat3 rmat = inverse_rotation();
    const glm::vec3 translation = (origin - rmat * position) / scale;

    return glm::mat4{glm::vec4(rmat[0] / scale, 0.f), glm::vec4(rmat[1] / scale, 0.f), glm::vec4(rmat[2] / scale, 0.f),
                     glm::vec4(translation, 1.f)};
}
glm::mat4 transform3D::scale_center_rotate_translate4() const
{
    const glm::vec3 translation = position - rotation * origin;

    return glm::mat4{glm::vec4(rotation[0] * scale.x, 0.f), glm::vec4(rotation[1] * scale.y, 0.f),
                     glm::vec4(rotation[2] * scale.z, 0.f), glm::vec4(translation, 1.f)};
}

glm::mat3 transform3D::inverse_rotation() const
{
    return glm::transpose(rotation);
}

void transform3D::translate_local(const glm::vec3 &dpos)
{
    position += rotation * dpos;
}
void transform3D::xtranslate_local(float dx)
{
    position += rotation[0] * dx;
}
void transform3D::ytranslate_local(float dy)
{
    position += rotation[1] * dy;
}
void transform3D::ztranslate_local(float dz)
{
    position += rotation[2] * dz;
}

void transform3D::rotate_local(const glm::mat3 &rotmat)
{
    rotation *= rotmat;
}
void transform3D::rotate_global(const glm::mat3 &rotmat)
{
    rotation = rotmat * rotation;
}

transform3D::trigonometry transform3D::trigonometric_functions(const glm::vec3 &rotation)
{
    return {cosf(rotation.x), sinf(rotation.x), cosf(rotation.y), sinf(rotation.y), cosf(rotation.z), sinf(rotation.z)};
}

glm::mat3 transform3D::XYZ(const glm::vec3 &rotation)
{
    const auto [c1, s1, c2, s2, c3, s3] = trigonometric_functions(rotation);
    return {{(c2 * c3), (c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2)},
            {(-c2 * s3), (c1 * c3 + s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3)},
            {(s2), (-c2 * s1), (c1 * c2)}};
}

glm::mat3 transform3D::XZY(const glm::vec3 &rotation)
{
    const auto [c1, s1, c3, s3, c2, s2] = trigonometric_functions(rotation);
    return {{(c2 * c3), (s1 * s3 + c1 * c3 * s2), (c3 * s1 * s2 - c1 * s3)},
            {(-s2), (c1 * c2), (c2 * s1)},
            {(c2 * s3), (c1 * s2 * s3 - c3 * s1), (c1 * c3 + s1 * s2 * s3)}};
}

glm::mat3 transform3D::YXZ(const glm::vec3 &rotation)
{
    const auto [c2, s2, c1, s1, c3, s3] = trigonometric_functions(rotation);
    return {{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)},
            {(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)},
            {(c2 * s1), (-s2), (c1 * c2)}};
}

glm::mat3 transform3D::YZX(const glm::vec3 &rotation)
{
    const auto [c3, s3, c1, s1, c2, s2] = trigonometric_functions(rotation);
    return {{(c1 * c2), (s2), (-c2 * s1)},
            {(s1 * s3 - c1 * c3 * s2), (c2 * c3), (c1 * s3 + c3 * s1 * s2)},
            {(c3 * s1 + c1 * s2 * s3), (-c2 * s3), (c1 * c3 - s1 * s2 * s3)}};
}

glm::mat3 transform3D::ZXY(const glm::vec3 &rotation)
{
    const auto [c2, s2, c3, s3, c1, s1] = trigonometric_functions(rotation);
    return {{(c1 * c3 - s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3), (-c2 * s3)},
            {(-c2 * s1), (c1 * c2), (s2)},
            {(c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2), (c2 * c3)}};
}

glm::mat3 transform3D::ZYX(const glm::vec3 &rotation)
{
    const auto [c3, s3, c2, s2, c1, s1] = trigonometric_functions(rotation);
    return {{(c1 * c2), (c2 * s1), (-s2)},
            {(c1 * s2 * s3 - c3 * s1), (c1 * c3 + s1 * s2 * s3), (c2 * s3)},
            {(s1 * s3 + c1 * c3 * s2), (c3 * s1 * s2 - c1 * s3), (c2 * c3)}};
}

glm::mat3 transform3D::XY(const float rotx, const float roty)
{
    const auto [c1, s1] = transform2D::trigonometric_functions(rotx);
    const auto [c2, s2] = transform2D::trigonometric_functions(roty);
    return {{c2, s1 * s2, -c1 * s2}, {0.f, c1, s1}, {s2, -c2 * s1, c1 * c2}};
}

glm::mat3 transform3D::XZ(const float rotx, const float rotz)
{
    const auto [c1, s1] = transform2D::trigonometric_functions(rotx);
    const auto [c3, s3] = transform2D::trigonometric_functions(rotz);
    return {{c3, c1 * s3, s1 * s3}, {-s3, c1 * c3, c3 * s1}, {0.f, -s1, c1}};
}

glm::mat3 transform3D::YX(const float roty, const float rotx)
{
    const auto [c1, s1] = transform2D::trigonometric_functions(rotx);
    const auto [c2, s2] = transform2D::trigonometric_functions(roty);
    return {{c2, 0.f, -s2}, {s1 * s2, c1, c2 * s1}, {c1 * s2, -s1, c1 * c2}};
}

glm::mat3 transform3D::YZ(float roty, float rotz)
{
    const auto [c2, s2] = transform2D::trigonometric_functions(roty);
    const auto [c3, s3] = transform2D::trigonometric_functions(rotz);
    return {{c2 * c3, s3, -c3 * s2}, {-c2 * s3, c3, s2 * s3}, {s2, 0.f, c2}};
}

glm::mat3 transform3D::ZX(const float rotz, const float rotx)
{
    const auto [c1, s1] = transform2D::trigonometric_functions(rotx);
    const auto [c3, s3] = transform2D::trigonometric_functions(rotz);
    return {{c3, s3, 0.f}, {-c1 * s3, c1 * c3, s1}, {s1 * s3, -c3 * s1, c1}};
}

glm::mat3 transform3D::ZY(float rotz, float roty)
{
    const auto [c2, s2] = transform2D::trigonometric_functions(roty);
    const auto [c3, s3] = transform2D::trigonometric_functions(rotz);
    return {{c2 * c3, c2 * s3, -s2}, {-s3, c3, 0.f}, {c3 * s2, s2 * s3, c2}};
}

glm::mat3 transform3D::X(const float rotx)
{
    const auto [c, s] = transform2D::trigonometric_functions(rotx);
    return {{1.f, 0.f, 0.f}, {0.f, c, s}, {0.f, -s, c}};
}

glm::mat3 transform3D::Y(const float roty)
{
    const auto [c, s] = transform2D::trigonometric_functions(roty);
    return {{c, 0.f, -s}, {0.f, 1.f, 0.f}, {s, 0.f, c}};
}

glm::mat3 transform3D::Z(const float rotz)
{
    const auto [c, s] = transform2D::trigonometric_functions(rotz);
    return {{c, s, 0.f}, {-s, c, 0.f}, {0.f, 0.f, 1.f}};
}

#ifdef KIT_USE_YAML_CPP
YAML::Node transform3D::serializer::encode(const transform3D &transform) const
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
bool transform3D::serializer::decode(const YAML::Node &node, transform3D &transform) const
{
    if (!node.IsMap() || node.size() != 6)
        return false;
    transform.position = node["Position"].as<glm::vec3>();
    transform.scale = node["Scale"].as<glm::vec3>();
    transform.origin = node["Origin"].as<glm::vec3>();
    transform.rotation[0] = node["XRotation"].as<glm::vec3>();
    transform.rotation[1] = node["YRotation"].as<glm::vec3>();
    transform.rotation[2] = node["ZRotation"].as<glm::vec3>();
    return true;
}
#endif

} // namespace kit