#pragma once
#include "Core/Scene/KitGameObject.h"
#include "Graphics/KitCamera.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitPipeline.h"
#include "KitFrameInfo.h"
#include "Graphics/KitGlobalGraphicsDefines.h"

namespace Kitsune
{
    class KitGizmoBillboardRenderSystem
    {
        KitEngineDevice* engine_device_;

        std::unique_ptr<KitPipeline> pipeline_;
        VkPipelineLayout             pipeline_layout_;

    public:
        KitGizmoBillboardRenderSystem(KitEngineDevice* device, VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout);
        ~KitGizmoBillboardRenderSystem();

        void Update(const KitFrameInfo &frame_info, KitGlobalUBO &ubo);
        void RenderGameObjects(const KitFrameInfo& frame_info, std::vector<KitGameObject>& game_objects) const;

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout);
        void CreatePipeline(VkRenderPass render_pass);
    };
}
