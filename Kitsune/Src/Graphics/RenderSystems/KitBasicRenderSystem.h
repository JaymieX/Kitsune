#pragma once

#include "KitRenderSystemBase.h"

namespace Kitsune
{
    class KitBasicRenderSystem : public KitRenderSystemBase
    {
    public:
        explicit KitBasicRenderSystem(KitEngineDevice* device);

        void Render(const KitFrameInfo& frame_info) const override;

    protected:
        void CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout) override;
        void CreatePipeline(VkRenderPass render_pass) override;
    };
}
