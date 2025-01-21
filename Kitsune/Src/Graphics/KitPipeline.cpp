#include "KitPipeline.h"

#include <fstream>

#include "Core/KitLogs.h"

namespace
{
    std::vector<char> ReadFile(const std::string& file_path)
    {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);

        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, file.is_open(), "File: {} was not opened!", file_path);

        const size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();

        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, !buffer.empty(), "File: {} could not be properly read!", file_path);

        return buffer;
    }
}

namespace Kitsune
{
    KitPipeline::KitPipeline(
        KitEngineDevice* device,
        const std::string& vert_path,
        const std::string& frag_path,
        const PipelineConfigInfo& pipeline_config_info):
        device_(device)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, pipeline_config_info.pipeline_layout != VK_NULL_HANDLE, "Graphic pipeline layout cannot be NULL!");
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, pipeline_config_info.render_pass     != VK_NULL_HANDLE, "Graphic render pass cannot be NULL!");
        
        std::vector<char> vert_code_buffer = ReadFile(vert_path);
        std::vector<char> frag_code_buffer = ReadFile(frag_path);

        CreateShaderModule(vert_code_buffer, &vert_shader_module_);
        CreateShaderModule(frag_code_buffer, &frag_shader_module_);

        VkPipelineShaderStageCreateInfo shader_stages[2];
        shader_stages[0].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage               = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module              = vert_shader_module_;
        shader_stages[0].pName               = "main";
        shader_stages[0].flags               = 0;
        shader_stages[0].pNext               = nullptr;
        shader_stages[0].pSpecializationInfo = nullptr;

        shader_stages[1].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module              = frag_shader_module_;
        shader_stages[1].pName               = "main";
        shader_stages[1].flags               = 0;
        shader_stages[1].pNext               = nullptr;
        shader_stages[1].pSpecializationInfo = nullptr;
        
        //std::vector<VkVertexInputAttributeDescription> attr_desc  = APModel::APVertex::GetAttributeDescriptions();
        //std::vector<VkVertexInputBindingDescription> binding_desc = APModel::APVertex::GetBindingDescriptions();
        
        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
        vertex_input_state_create_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state_create_info.vertexAttributeDescriptionCount = 0; //static_cast<uint32_t>(attr_desc.size());
        vertex_input_state_create_info.vertexBindingDescriptionCount   = 0; //static_cast<uint32_t>(binding_desc.size());
        vertex_input_state_create_info.pVertexAttributeDescriptions    = nullptr; //attr_desc.data();
        vertex_input_state_create_info.pVertexBindingDescriptions      = nullptr; //binding_desc.data();

        VkPipelineViewportStateCreateInfo viewport_config_info{};
        viewport_config_info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_config_info.viewportCount = 1;
        viewport_config_info.pViewports    = &pipeline_config_info.viewport;
        viewport_config_info.scissorCount  = 1;
        viewport_config_info.pScissors     = &pipeline_config_info.scissor;

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info{};
        graphics_pipeline_create_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_create_info.stageCount          = 2;
        graphics_pipeline_create_info.pStages             = shader_stages;
        graphics_pipeline_create_info.pVertexInputState   = &vertex_input_state_create_info;
        graphics_pipeline_create_info.pInputAssemblyState = &pipeline_config_info.input_assembly_info;
        graphics_pipeline_create_info.pViewportState      = &viewport_config_info;
        graphics_pipeline_create_info.pRasterizationState = &pipeline_config_info.rasterization_info;
        graphics_pipeline_create_info.pColorBlendState    = &pipeline_config_info.color_blend_info;
        graphics_pipeline_create_info.pMultisampleState   = &pipeline_config_info.multisample_info;
        graphics_pipeline_create_info.pDepthStencilState  = &pipeline_config_info.depth_stencil_info;
        graphics_pipeline_create_info.pDynamicState       = nullptr;
        graphics_pipeline_create_info.layout              = pipeline_config_info.pipeline_layout;
        graphics_pipeline_create_info.renderPass          = pipeline_config_info.render_pass;
        graphics_pipeline_create_info.subpass             = pipeline_config_info.subpass;
        graphics_pipeline_create_info.basePipelineIndex   = -1;
        graphics_pipeline_create_info.basePipelineHandle  = VK_NULL_HANDLE;

        VkResult result = vkCreateGraphicsPipelines(device_->GetDevice(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &graphics_pipeline_);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create graphic pipeline!");
    }

    KitPipeline::~KitPipeline()
    {
        vkDestroyShaderModule(device_->GetDevice(), vert_shader_module_, nullptr);
        vkDestroyShaderModule(device_->GetDevice(), frag_shader_module_, nullptr);

        vkDestroyPipeline(device_->GetDevice(), graphics_pipeline_, nullptr);
    }

    PipelineConfigInfo KitPipeline::DefaultPipelineConfigInfo(const uint32_t width, const uint32_t height)
    {
        PipelineConfigInfo config_info{};
        config_info.input_assembly_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config_info.input_assembly_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;
        
        config_info.viewport.x        = 0.0f;
        config_info.viewport.y        = 0.0f;
        config_info.viewport.width    = static_cast<float>(width);
        config_info.viewport.height   = static_cast<float>(height);
        config_info.viewport.minDepth = 0.0f;
        config_info.viewport.maxDepth = 1.0f;
        
        config_info.scissor.offset = {0, 0};
        config_info.scissor.extent = {width, height};

        config_info.rasterization_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config_info.rasterization_info.depthClampEnable        = VK_FALSE;
        config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        config_info.rasterization_info.polygonMode             = VK_POLYGON_MODE_FILL;
        config_info.rasterization_info.lineWidth               = 1.0f;
        config_info.rasterization_info.cullMode                = VK_CULL_MODE_NONE; // Use back cull when possible
        config_info.rasterization_info.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        config_info.rasterization_info.depthBiasEnable         = VK_FALSE;
        config_info.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
        config_info.rasterization_info.depthBiasClamp          = 0.0f;  // Optional
        config_info.rasterization_info.depthBiasSlopeFactor    = 0.0f;  // Optional

        config_info.multisample_info.sType                     = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config_info.multisample_info.sampleShadingEnable       = VK_FALSE;
        config_info.multisample_info.rasterizationSamples      = VK_SAMPLE_COUNT_1_BIT;
        config_info.multisample_info.minSampleShading          = 1.0f;      // Optional
        config_info.multisample_info.pSampleMask               = nullptr;   // Optional
        config_info.multisample_info.alphaToCoverageEnable     = VK_FALSE;  // Optional
        config_info.multisample_info.alphaToOneEnable          = VK_FALSE;  // Optional

        config_info.color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        config_info.color_blend_attachment.blendEnable         = VK_FALSE;
        config_info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config_info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config_info.color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;       // Optional
        config_info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config_info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config_info.color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;       // Optional
        
        config_info.color_blend_info.sType                     = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config_info.color_blend_info.logicOpEnable             = VK_FALSE;
        config_info.color_blend_info.logicOp                   = VK_LOGIC_OP_COPY;  // Optional
        config_info.color_blend_info.attachmentCount           = 1;
        config_info.color_blend_info.pAttachments              = &config_info.color_blend_attachment;
        config_info.color_blend_info.blendConstants[0]         = 0.0f;  // Optional
        config_info.color_blend_info.blendConstants[1]         = 0.0f;  // Optional
        config_info.color_blend_info.blendConstants[2]         = 0.0f;  // Optional
        config_info.color_blend_info.blendConstants[3]         = 0.0f;  // Optional

        config_info.depth_stencil_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config_info.depth_stencil_info.depthTestEnable         = VK_TRUE;
        config_info.depth_stencil_info.depthWriteEnable        = VK_TRUE;
        config_info.depth_stencil_info.depthCompareOp          = VK_COMPARE_OP_LESS;
        config_info.depth_stencil_info.depthBoundsTestEnable   = VK_FALSE;
        config_info.depth_stencil_info.minDepthBounds          = 0.0f;  // Optional
        config_info.depth_stencil_info.maxDepthBounds          = 1.0f;  // Optional
        config_info.depth_stencil_info.stencilTestEnable       = VK_FALSE;
        config_info.depth_stencil_info.front                   = {};  // Optional
        config_info.depth_stencil_info.back                    = {};  // Optional
        
        return config_info;
    }

    void KitPipeline::Bind(const VkCommandBuffer command_buffer) const
    {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
    }

    void KitPipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* module) const
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

        VkResult result = vkCreateShaderModule(device_->GetDevice(), &create_info, nullptr, module);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create shader module");
    }
}
