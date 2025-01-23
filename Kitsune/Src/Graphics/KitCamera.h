#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Core/KitDefinitions.h"

namespace Kitsune
{
    class KitCamera
    {
        glm::mat4 projection_matrix_ {1.f};
        glm::mat4 view_matrix_       {1.f};
        
    public:

        KIT_NODISCARD const glm::mat4& GetProjectionMatrix() const { return projection_matrix_; }
        KIT_NODISCARD const glm::mat4& GetViewMatrix() const { return view_matrix_; }
        
        void SetOrthographicProjectionMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far);
        void SetPerspectiveProjectionMatrix(const float fov_y, const float aspect_ratio, const float near, const float far);

        void SetViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = {0.f, -1.f, 0.f});
        void SetViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = {0.f, -1.f, 0.f});
        void SetViewYXZ(const glm::vec3& position, const glm::vec3& rotation);
    };
}
