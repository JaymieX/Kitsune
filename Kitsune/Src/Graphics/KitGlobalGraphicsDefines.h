#pragma once
#include <glm/glm.hpp>

namespace Kitsune
{
    struct KitPushConstantsData
    {
        glm::mat4 model_matrix{1.f}; // Default id matrix init
        glm::mat4 normal_matrix{1.f};
    };

    struct KitGlobalUBO
    {
        glm::mat4             projection{1.f};
        glm::mat4             view{1.f};
        alignas(16) glm::vec3 light_direction = glm::normalize(glm::vec3(1.f, -3.f, -1.f));

        alignas(16) glm::vec4 ambient_color        = glm::vec4(1.f, 1.f, 1.f, .02f);
        alignas(16) glm::vec3 point_light_position = glm::vec3(-1.f);
        alignas(16) glm::vec4 point_light_color    = glm::vec4(1.f, .5f, .2f, 1.f);
    };
}
