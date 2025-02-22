#pragma once
#include <glm/glm.hpp>

namespace Kitsune
{
    struct KitGlobalUBO
    {
        glm::mat4 projection_view{1.f};
        glm::vec3 light_direction = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
    };
}
