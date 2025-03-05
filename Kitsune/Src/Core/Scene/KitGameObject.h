#pragma once
#include "Core/KitDefinitions.h"

#include "Graphics/KitModel.h"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include "Components/KitLightComponents.h"
#include "glm/gtc/matrix_inverse.hpp"

namespace Kitsune
{
    struct KitTransform
    {
        glm::vec3 translation;
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation;

        glm::mat4 ToMatrix() const
        {
            auto transform = glm::translate(glm::mat4(1.0f), translation);

            transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
            transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
            transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});

            transform = glm::scale(transform, scale);

            return transform;
        }

        glm::mat3 GetNormalMatrix() const
        {
            const glm::mat3x3 model_matrix3(ToMatrix());
            return glm::inverseTranspose(model_matrix3);
        }
    };

    class KitGameObject
    {
    public:
        using KitGameObjID = unsigned int;

    private:
        KitGameObjID id_;

        explicit KitGameObject(const KitGameObjID id);

    public:
        std::shared_ptr<KitModel> model;
        glm::vec3                 color;

        KitTransform transform;

        std::shared_ptr<KitPointLightComponent> point_light_component = nullptr;

        static KitGameObject CreateGameObject()
        {
            static KitGameObjID id = 0;
            return KitGameObject(++id);
        }

        static KitGameObject CreatePointLight(
            const float      intensity = 10.f,
            const float      radius    = 0.1f,
            const glm::vec3& color     = glm::vec3(1.0f, 1.0f, 1.0f))
        {
            KitGameObject result         = CreateGameObject();
            result.color                 = color;
            result.transform.scale.x     = radius;
            result.point_light_component = std::make_shared<KitPointLightComponent>(intensity);

            return result;
        }

        KIT_NODISCARD KitGameObjID GetId() const { return id_; }
    };
}
