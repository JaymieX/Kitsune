#pragma once
#include "Core/Scene/KitGameObject.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitPipeline.h"

namespace Kitsune
{
    struct KitPushConstantsData
    {
        glm::mat2   transform{1.f}; // Default id matrix init
        alignas(8)  glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };
    
    class BasicRenderSystem
    {
        KitEngineDevice* engine_device_;

        std::unique_ptr<KitPipeline> pipeline_;
        VkPipelineLayout pipeline_layout_;
        
    public:
        BasicRenderSystem(KitEngineDevice* device, VkRenderPass render_pass);
        ~BasicRenderSystem();

        void RenderGameObjects(VkCommandBuffer command_buffer, const std::vector<KitGameObject>& game_objects) const;

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass render_pass);
    };
}
