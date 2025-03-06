#pragma once

#include <memory>

#include "KitFrameInfo.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitGlobalGraphicsDefines.h"
#include "Graphics/KitPipeline.h"

namespace Kitsune
{
    class KitRenderSystemBase
    {
    protected:
        KitEngineDevice* engine_device_;

        std::unique_ptr<KitPipeline> pipeline_        = nullptr;
        VkPipelineLayout             pipeline_layout_ = nullptr;

        explicit KitRenderSystemBase(
            KitEngineDevice* device) :
            engine_device_(device)
        {
        }

    public:
        virtual ~KitRenderSystemBase()
        {
            vkDestroyPipelineLayout(engine_device_->GetDevice(), pipeline_layout_, nullptr);
        }

        virtual void Init(
            const VkRenderPass          render_pass,
            const VkDescriptorSetLayout descriptor_set_layout)
        {
            CreatePipelineLayout(descriptor_set_layout);
            CreatePipeline(render_pass);
        }

        virtual void Update(const KitFrameInfo& frame_info, KitGlobalUBO& ubo)
        {
        };

        virtual void Render(const KitFrameInfo& frame_info) const = 0;

    protected:
        virtual void CreatePipelineLayout(const VkDescriptorSetLayout descriptor_set_layout) = 0;
        virtual void CreatePipeline(const VkRenderPass render_pass) = 0;
    };
}
