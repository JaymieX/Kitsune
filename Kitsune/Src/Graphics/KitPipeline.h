#pragma once

#include <vulkan/vulkan_core.h>

#include "KitEngineDevice.h"

namespace Kitsune
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo color_blend_info;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
        VkPipelineLayout pipeline_layout = nullptr;
        VkRenderPass render_pass = nullptr;
        uint32_t subpass = 0;
    };
    
    class KitPipeline
    {
        KitEngineDevice* device_ = nullptr;
        
        VkPipeline graphics_pipeline_ = nullptr;
        
        VkShaderModule vert_shader_module_ = nullptr;
        VkShaderModule frag_shader_module_ = nullptr;
        
    public:
        KitPipeline(KitEngineDevice* device, const std::string& vert_path, const std::string& frag_path, const PipelineConfigInfo& pipeline_config_info);
        ~KitPipeline();

        KitPipeline(const KitPipeline&) = delete;
        KitPipeline(KitPipeline&&)      = delete;
        
        KitPipeline& operator=(const KitPipeline&) = delete;
        KitPipeline& operator=(KitPipeline&&)      = delete;

        static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);

        void Bind(const VkCommandBuffer command_buffer) const;

    private:
        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* module) const;
    };
}
