#pragma once
#include "Core/Scene/KitGameObject.h"
#include "Graphics/KitCamera.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitPipeline.h"
#include "KitFrameInfo.h"

namespace Kitsune
{
    struct KitPushConstantsData
    {
        glm::mat4 model_matrix{1.f}; // Default id matrix init
        glm::mat4 normal_matrix{1.f};
    };

    class KitBasicRenderSystem
    {
        KitEngineDevice* engine_device_;

        std::unique_ptr<KitPipeline> pipeline_;
        VkPipelineLayout             pipeline_layout_;

    public:
        KitBasicRenderSystem(KitEngineDevice* device, VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout);
        ~KitBasicRenderSystem();

        void RenderGameObjects(const KitFrameInfo& frame_info, std::vector<KitGameObject>& game_objects) const;

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout);
        void CreatePipeline(VkRenderPass render_pass);
    };
}
