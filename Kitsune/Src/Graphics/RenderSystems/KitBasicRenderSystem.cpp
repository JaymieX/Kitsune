#include "KitBasicRenderSystem.h"

#include <ranges>

#include "Core/KitLogs.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace Kitsune
{
    KitBasicRenderSystem::KitBasicRenderSystem(
        KitEngineDevice*      device,
        VkRenderPass          render_pass,
        VkDescriptorSetLayout descriptor_set_layout):
        engine_device_(device)
    {
        CreatePipelineLayout(descriptor_set_layout);
        CreatePipeline(render_pass);
    }

    KitBasicRenderSystem::~KitBasicRenderSystem()
    {
        vkDestroyPipelineLayout(engine_device_->GetDevice(), pipeline_layout_, nullptr);
    }

    void KitBasicRenderSystem::RenderGameObjects(const KitFrameInfo& frame_info, std::vector<KitGameObject>& game_objects) const
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

        for (auto& game_obj : game_objects)
        {
            KitPushConstantsData push_constants_data{};
            push_constants_data.model_matrix  = game_obj.transform.ToMatrix();
            push_constants_data.normal_matrix = game_obj.transform.GetNormalMatrix();

            vkCmdPushConstants(
                frame_info.command_buffer,
                pipeline_layout_,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(push_constants_data),
                &push_constants_data);
            game_obj.model->Bind(frame_info.command_buffer);
            game_obj.model->Draw(frame_info.command_buffer);
        }
    }

    void KitBasicRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout)
    {
        VkPushConstantRange push_constant_range;
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset     = 0;
        push_constant_range.size       = sizeof(KitPushConstantsData);

        std::vector<VkDescriptorSetLayout> set_layouts_sets{descriptor_set_layout};

        VkPipelineLayoutCreateInfo create_info{};
        create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        create_info.setLayoutCount         = set_layouts_sets.size();
        create_info.pSetLayouts            = set_layouts_sets.data();
        create_info.pushConstantRangeCount = 1;
        create_info.pPushConstantRanges    = &push_constant_range;

        VkResult result = vkCreatePipelineLayout(engine_device_->GetDevice(), &create_info, nullptr, &pipeline_layout_);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create pipeline layout!");
    }

    void KitBasicRenderSystem::CreatePipeline(VkRenderPass render_pass)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, pipeline_layout_ != nullptr, "Pipeline layouts do not exist at pipeline creation!");

        PipelineConfigInfo pipeline_config{};
        KitPipeline::DefaultPipelineConfigInfo(pipeline_config);

        pipeline_config.render_pass     = render_pass;
        pipeline_config.pipeline_layout = pipeline_layout_;

        pipeline_ = std::make_unique<KitPipeline>(
            engine_device_,
            "Shader/Simple3DVert.spv",
            "Shader/Simple3DFrag.spv",
            pipeline_config);
    }
}
