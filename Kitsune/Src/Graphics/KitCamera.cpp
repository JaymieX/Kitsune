﻿#include "KitCamera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Kitsune
{
    void KitCamera::SetOrthographicProjectionMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far)
    {
        projection_matrix_ = glm::mat4{1.0f};
        projection_matrix_[0][0] = 2.f / (right - left);
        projection_matrix_[1][1] = 2.f / (bottom - top);
        projection_matrix_[2][2] = 1.f / (far - near);
        projection_matrix_[3][0] = -(right + left) / (right - left);
        projection_matrix_[3][1] = -(bottom + top) / (bottom - top);
        projection_matrix_[3][2] = -near / (far - near);
    }

    void KitCamera::SetPerspectiveProjectionMatrix(const float fov_y, const float aspect_ratio, const float near, const float far)
    {
        const float tanHalfFovy = tan(fov_y / 2.f);
        projection_matrix_ = glm::mat4{0.0f};
        projection_matrix_[0][0] = 1.f / (aspect_ratio * tanHalfFovy);
        projection_matrix_[1][1] = 1.f / (tanHalfFovy);
        projection_matrix_[2][2] = far / (far - near);
        projection_matrix_[2][3] = 1.f;
        projection_matrix_[3][2] = -(far * near) / (far - near);
    }

    void KitCamera::SetViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up)
    {
        const glm::vec3 w{glm::normalize(direction)};
        const glm::vec3 u{glm::normalize(glm::cross(w, up))};
        const glm::vec3 v{glm::cross(w, u)};

        view_matrix_ = glm::mat4{1.f};
        view_matrix_[0][0] = u.x;
        view_matrix_[1][0] = u.y;
        view_matrix_[2][0] = u.z;
        view_matrix_[0][1] = v.x;
        view_matrix_[1][1] = v.y;
        view_matrix_[2][1] = v.z;
        view_matrix_[0][2] = w.x;
        view_matrix_[1][2] = w.y;
        view_matrix_[2][2] = w.z;
        view_matrix_[3][0] = -glm::dot(u, position);
        view_matrix_[3][1] = -glm::dot(v, position);
        view_matrix_[3][2] = -glm::dot(w, position);

        inverse_view_matrix_ = glm::mat4{1.f};
        inverse_view_matrix_[0][0] = u.x;
        inverse_view_matrix_[0][1] = u.y;
        inverse_view_matrix_[0][2] = u.z;
        inverse_view_matrix_[1][0] = v.x;
        inverse_view_matrix_[1][1] = v.y;
        inverse_view_matrix_[1][2] = v.z;
        inverse_view_matrix_[2][0] = w.x;
        inverse_view_matrix_[2][1] = w.y;
        inverse_view_matrix_[2][2] = w.z;
        inverse_view_matrix_[3][0] = position.x;
        inverse_view_matrix_[3][1] = position.y;
        inverse_view_matrix_[3][2] = position.z;
    }

    void KitCamera::SetViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        SetViewDirection(position, target - position, up);
    }

    void KitCamera::SetViewYXZ(const glm::vec3& position, const glm::vec3& rotation)
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        view_matrix_ = glm::mat4{1.f};
        view_matrix_[0][0] = u.x;
        view_matrix_[1][0] = u.y;
        view_matrix_[2][0] = u.z;
        view_matrix_[0][1] = v.x;
        view_matrix_[1][1] = v.y;
        view_matrix_[2][1] = v.z;
        view_matrix_[0][2] = w.x;
        view_matrix_[1][2] = w.y;
        view_matrix_[2][2] = w.z;
        view_matrix_[3][0] = -glm::dot(u, position);
        view_matrix_[3][1] = -glm::dot(v, position);
        view_matrix_[3][2] = -glm::dot(w, position);

        inverse_view_matrix_ = glm::mat4{1.f};
        inverse_view_matrix_[0][0] = u.x;
        inverse_view_matrix_[0][1] = u.y;
        inverse_view_matrix_[0][2] = u.z;
        inverse_view_matrix_[1][0] = v.x;
        inverse_view_matrix_[1][1] = v.y;
        inverse_view_matrix_[1][2] = v.z;
        inverse_view_matrix_[2][0] = w.x;
        inverse_view_matrix_[2][1] = w.y;
        inverse_view_matrix_[2][2] = w.z;
        inverse_view_matrix_[3][0] = position.x;
        inverse_view_matrix_[3][1] = position.y;
        inverse_view_matrix_[3][2] = position.z;
    }
}
