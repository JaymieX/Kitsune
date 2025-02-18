#pragma once
#include "Core/Scene/KitGameObject.h"
#include "Graphics/KitCamera.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitPipeline.h"

namespace Kitsune
{
    struct KitPushConstantsData
    {
        glm::mat4 transform{1.f}; // Default id matrix init
        glm::mat4 normal_matrix{1.f};
    };
    
    class KitBasicRenderSystem
    {
        KitEngineDevice* engine_device_;

        std::unique_ptr<KitPipeline> pipeline_;
        VkPipelineLayout pipeline_layout_;
        
    public:
        KitBasicRenderSystem(KitEngineDevice* device, VkRenderPass render_pass);
        ~KitBasicRenderSystem();

        void RenderGameObjects(VkCommandBuffer command_buffer, std::vector<KitGameObject>& game_objects, const KitCamera& camera) const;

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass render_pass);
    };
}
