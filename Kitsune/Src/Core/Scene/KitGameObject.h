#pragma once
#include "Core/KitDefinitions.h"

#include "Graphics/KitModel.h"

#include <memory>

namespace Kitsune
{
    struct KitTransform2d
    {
        glm::vec2 translation;
        glm::vec2 scale{1.f, 1.f};
        float rotation = 0.f;

        glm::mat2 ToMatrix() const
        {
            const float sin = glm::sin(rotation);
            const float cos = glm::cos(rotation);
            glm::mat2 rot_matrix{{cos, sin}, {-sin, cos}};
            
            glm::mat2 scale_mat{{scale.x, .0f}, {.0f, scale.y}};
            
            return rot_matrix * scale_mat;
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

        KitTransform2d transform2d;
        
        static KitGameObject CreateGameObject()
        {
            static KitGameObjID id = 0;
            return KitGameObject(++id);
        }

        KIT_NODISCARD KitGameObjID GetId() const { return id_; }
    };
}
