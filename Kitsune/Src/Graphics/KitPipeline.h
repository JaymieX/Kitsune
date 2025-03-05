#pragma once

#include <vulkan/vulkan_core.h>

#include "KitEngineDevice.h"

namespace Kitsune
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo()                                     = default;
        PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription>   vertex_input_binding_descriptions;
        std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_descriptions;
        VkPipelineViewportStateCreateInfo              viewport_state_info;
        VkPipelineInputAssemblyStateCreateInfo         input_assembly_info;
        VkPipelineRasterizationStateCreateInfo         rasterization_info;
        VkPipelineMultisampleStateCreateInfo           multisample_info;
        VkPipelineColorBlendAttachmentState            color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo            color_blend_info;
        VkPipelineDepthStencilStateCreateInfo          depth_stencil_info;
        std::vector<VkDynamicState>                    dynamic_state_enables;
        VkPipelineDynamicStateCreateInfo               dynamic_state_info;
        VkPipelineLayout                               pipeline_layout = nullptr;
        VkRenderPass                                   render_pass     = nullptr;
        uint32_t                                       subpass         = 0;
    };

    class KitPipeline
    {
        KitEngineDevice* device_ = nullptr;

        VkPipeline graphics_pipeline_ = nullptr;

        VkShaderModule vert_shader_module_ = nullptr;
        VkShaderModule frag_shader_module_ = nullptr;

    public:
        KitPipeline(
            KitEngineDevice*          device,
            const std::string&        vert_path,
            const std::string&        frag_path,
            const PipelineConfigInfo& pipeline_config_info);
        ~KitPipeline();

        KitPipeline(const KitPipeline&) = delete;
        KitPipeline(KitPipeline&&)      = delete;

        KitPipeline& operator=(const KitPipeline&) = delete;
        KitPipeline& operator=(KitPipeline&&)      = delete;

        static void DefaultPipelineConfigInfo(PipelineConfigInfo& config_info);

        void Bind(const VkCommandBuffer command_buffer) const;

    private:
        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* module) const;
    };
}
