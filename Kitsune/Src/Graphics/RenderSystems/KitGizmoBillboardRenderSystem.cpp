#include "KitGizmoBillboardRenderSystem.h"

#include <ranges>

#include "Core/KitLogs.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace Kitsune
{
    KitGizmoBillboardRenderSystem::KitGizmoBillboardRenderSystem(
        KitEngineDevice*      device,
        VkRenderPass          render_pass,
        VkDescriptorSetLayout descriptor_set_layout):
        engine_device_(device)
    {
        CreatePipelineLayout(descriptor_set_layout);
        CreatePipeline(render_pass);
    }

    KitGizmoBillboardRenderSystem::~KitGizmoBillboardRenderSystem()
    {
        vkDestroyPipelineLayout(engine_device_->GetDevice(), pipeline_layout_, nullptr);
    }

    void KitGizmoBillboardRenderSystem::RenderGameObjects(const KitFrameInfo& frame_info, std::vector<KitGameObject>& game_objects) const
    {
        pipeline_->Bind(frame_info.command_buffer);

        vkCmdBindDescriptorSets(
            frame_info.command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout_,
            0,
            1,
            &frame_info.descriptor_set,
            0,
            nullptr);

        vkCmdDraw(frame_info.command_buffer, 6, 1, 0, 0);
    }

    void KitGizmoBillboardRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout)
    {
        std::vector<VkDescriptorSetLayout> set_layouts_sets{descriptor_set_layout};

        VkPipelineLayoutCreateInfo create_info{};
        create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        create_info.setLayoutCount         = set_layouts_sets.size();
        create_info.pSetLayouts            = set_layouts_sets.data();
        create_info.pushConstantRangeCount = 0;
        create_info.pPushConstantRanges    = nullptr;

        VkResult result = vkCreatePipelineLayout(engine_device_->GetDevice(), &create_info, nullptr, &pipeline_layout_);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create pipeline layout!");
    }

    void KitGizmoBillboardRenderSystem::CreatePipeline(VkRenderPass render_pass)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, pipeline_layout_ != nullptr, "Pipeline layouts do not exist at pipeline creation!");

        PipelineConfigInfo pipeline_config{};
        KitPipeline::DefaultPipelineConfigInfo(pipeline_config);

        pipeline_config.vertex_input_attribute_descriptions.clear();
        pipeline_config.vertex_input_binding_descriptions.clear();

        pipeline_config.render_pass     = render_pass;
        pipeline_config.pipeline_layout = pipeline_layout_;

        pipeline_ = std::make_unique<KitPipeline>(
            engine_device_,
            "Shader/SimpleBillboardVert.spv",
            "Shader/SimpleBillboardFrag.spv",
            pipeline_config);
    }
}
