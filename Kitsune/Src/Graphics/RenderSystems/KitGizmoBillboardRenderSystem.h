#pragma once
#include "Core/Scene/KitGameObject.h"
#include "Graphics/KitCamera.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitPipeline.h"
#include "KitFrameInfo.h"
#include "KitRenderSystemBase.h"
#include "Graphics/KitGlobalGraphicsDefines.h"

namespace Kitsune
{
    class KitGizmoBillboardRenderSystem : public KitRenderSystemBase
    {
    public:
        explicit KitGizmoBillboardRenderSystem(KitEngineDevice* device);

        void Update(const KitFrameInfo &frame_info, KitGlobalUBO &ubo) override;
        void Render(const KitFrameInfo& frame_info) const override;

    protected:
        void CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout) override;
        void CreatePipeline(VkRenderPass render_pass) override;
    };
}
