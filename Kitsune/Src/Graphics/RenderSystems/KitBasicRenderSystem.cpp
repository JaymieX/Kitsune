#include "KitBasicRenderSystem.h"

#include <ranges>

#include "Core/KitLogs.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace Kitsune
{
    KitBasicRenderSystem::KitBasicRenderSystem(KitEngineDevice* device, VkRenderPass render_pass):
        engine_device_(device)
    {
        CreatePipelineLayout();
        CreatePipeline(render_pass);
    }

    KitBasicRenderSystem::~KitBasicRenderSystem()
    {
        vkDestroyPipelineLayout(engine_device_->GetDevice(), pipeline_layout_, nullptr);
    }

    void KitBasicRenderSystem::RenderGameObjects(VkCommandBuffer command_buffer, std::vector<KitGameObject>& game_objects, const KitCamera& camera) const
    {
        pipeline_->Bind(command_buffer);

        for (auto& game_obj : game_objects)
        {
            game_obj.transform.rotation.y = glm::mod(game_obj.transform.rotation.y + .001f, glm::two_pi<float>());
            game_obj.transform.rotation.z = glm::mod(game_obj.transform.rotation.z + .001f, glm::two_pi<float>());
            
            KitPushConstantsData push_constants_data{};
            push_constants_data.color     = game_obj.color;
            push_constants_data.transform = camera.GetProjectionMatrix() * game_obj.transform.ToMatrix();

            vkCmdPushConstants(
                command_buffer,
                pipeline_layout_,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(push_constants_data),
                &push_constants_data);

            game_obj.model->Bind(command_buffer);
            game_obj.model->Draw(command_buffer);
        }
    }

    void KitBasicRenderSystem::CreatePipelineLayout()
    {
        VkPushConstantRange push_constant_range;
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset     = 0;
        push_constant_range.size       = sizeof(KitPushConstantsData);
        
        VkPipelineLayoutCreateInfo create_info{};
        create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        create_info.setLayoutCount         = 0;
        create_info.pSetLayouts            = nullptr;
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

        pipeline_ = std::make_unique<KitPipeline>(engine_device_, "Shader/Simple3DVert.spv", "Shader/Simple3DFrag.spv", pipeline_config);
    }
}
