#pragma once
#include <glm/glm.hpp>

namespace Kitsune
{
#define MAX_LIGHTS 10

    struct PointLightData
    {
        glm::vec4 position;
        glm::vec4 color;
    };

    struct KitGlobalUBO
    {
        glm::mat4             projection{1.f};
        glm::mat4             view{1.f};
        glm::mat4             inverse_view{1.f};
        alignas(16) glm::vec3 light_direction = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
        float                 padding1;

        alignas(16) glm::vec4      ambient_color = glm::vec4(1.f, 1.f, 1.f, .02f);
        alignas(16) PointLightData point_lights[MAX_LIGHTS];
        alignas(16) int            num_lights;
        float                      padding2[3];
    };
}
