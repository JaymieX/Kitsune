#pragma once
#include "Core/KitDefinitions.h"

#include "Graphics/KitModel.h"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

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

            transform = glm::rotate(transform, rotation.y,{0.f, 1.f, 0.f} );
            transform = glm::rotate(transform, rotation.x,{1.f, 0.f, 0.f} );
            transform = glm::rotate(transform, rotation.z,{0.f, 0.f, 1.f} );
            
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
        glm::vec3 color;

        KitTransform transform;
        
        static KitGameObject CreateGameObject()
        {
            static KitGameObjID id = 0;
            return KitGameObject(++id);
        }

        KIT_NODISCARD KitGameObjID GetId() const { return id_; }
    };
}
