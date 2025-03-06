#include "KitGizmoBillboardRenderSystem.h"

#include <ranges>

#include "Core/KitLogs.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace Kitsune
{
    struct KitPushConstantsData
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    KitGizmoBillboardRenderSystem::KitGizmoBillboardRenderSystem(
        KitEngineDevice*      device):
        KitRenderSystemBase(device)
    {
    }

    void KitGizmoBillboardRenderSystem::Update(const KitFrameInfo& frame_info, KitGlobalUBO& ubo)
    {
        auto rotate_light = glm::rotate(glm::mat4(1.f), 0.5f * frame_info.frame_time, {0.f, -1.f, 0.f});
        int  light_index  = 0;
        for (auto& obj : frame_info.game_objects)
        {
            if (obj.point_light_component == nullptr)
            {
                continue;
            }

            assert(light_index < MAX_LIGHTS && "Point lights exceed maximum specified");

            // update light position
            obj.transform.translation = glm::vec3(rotate_light * glm::vec4(obj.transform.translation, 1.f));

            // copy light to ubo
            ubo.point_lights[light_index].position = glm::vec4(obj.transform.translation, 1.f);
            ubo.point_lights[light_index].color    = glm::vec4(obj.color, obj.point_light_component->light_intensity);

            light_index++;
        }

        ubo.num_lights = light_index;
    }

    void KitGizmoBillboardRenderSystem::Render(const KitFrameInfo& frame_info) const
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

        for (auto& obj : frame_info.game_objects)
        {
            if (obj.point_light_component == nullptr)
                continue;

            KitPushConstantsData push{};
            push.position = glm::vec4(obj.transform.translation, 1.f);
            push.color    = glm::vec4(obj.color, obj.point_light_component->light_intensity);
            push.radius   = obj.transform.scale.x;

            vkCmdPushConstants(
                frame_info.command_buffer,
                pipeline_layout_,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(KitPushConstantsData),
                &push);
            vkCmdDraw(frame_info.command_buffer, 6, 1, 0, 0);
        }
    }

    void KitGizmoBillboardRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptor_set_layout)
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
