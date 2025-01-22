#include "KitCamera.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Kitsune
{
    void KitCamera::SetOrthographicProjectionMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far)
    {
        projection_matrix_ = glm::ortho(left, right, bottom, top, near, far);
    }

    void KitCamera::SetPerspectiveProjectionMatrix(const float fov_y, const float aspect_ratio, const float near, const float far)
    {
        // If Vulkan use perspectiveZO
        projection_matrix_ = glm::perspectiveZO(fov_y, aspect_ratio, near, far);
        projection_matrix_[1][1] *= -1;
    }
}
