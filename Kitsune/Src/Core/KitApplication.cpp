#include "KitApplication.h"

#include <glm/vec2.hpp>

#include "KitLogs.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitModel.h"
#include "Graphics/KitPipeline.h"

namespace Kitsune
{
    KitApplication::KitApplication()
    {
        KitLog::InitLoggers();
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application starting...");

        window_ = std::make_unique<KitWindow>(KitWindowInfo(default_width, default_height, default_title));
        engine_device_ = std::make_unique<KitEngineDevice>(window_.get());
        swap_chain_ = std::make_unique<KitSwapChain>(engine_device_.get());

        LoadModel();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    KitApplication::~KitApplication()
    {
        vkDestroyPipelineLayout(engine_device_->GetDevice(), pipeline_layout_, nullptr);
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application closing...");
    }

    void KitApplication::Run()
    {
        while (!window_->ShouldClose())
        {
            glfwPollEvents();
            Draw();
        }

        engine_device_->DeviceWaitIdle();
    }

    void KitApplication::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo create_info{};
        create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        create_info.setLayoutCount         = 0;
        create_info.pSetLayouts            = nullptr;
        create_info.pushConstantRangeCount = 0;
        create_info.pPushConstantRanges    = nullptr;

        VkResult result = vkCreatePipelineLayout(engine_device_->GetDevice(), &create_info, nullptr, &pipeline_layout_);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create pipeline layout!");
    }

    void KitApplication::CreatePipeline()
    {
        PipelineConfigInfo pipeline_config = KitPipeline::DefaultPipelineConfigInfo(swap_chain_->Width(), swap_chain_->Height());
        
        pipeline_config.render_pass     = swap_chain_->GetRenderPass();
        pipeline_config.pipeline_layout = pipeline_layout_;

        pipeline_ = std::make_unique<KitPipeline>(engine_device_.get(), "Shader/Simple3DVert.spv", "Shader/Simple3DFrag.spv", pipeline_config);
    }

    void KitApplication::CreateCommandBuffers()
    {
        command_buffers_.resize(swap_chain_->ImageCount());
        KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, KitLogLevel::LOG_INFO, "Number of command buffers to create: {}", command_buffers_.size());

        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandPool        = engine_device_->GetCommandPool();
        allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

        VkResult result = vkAllocateCommandBuffers(engine_device_->GetDevice(), &allocate_info, command_buffers_.data());
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create command buffers!");

        for (int i = 0; i < command_buffers_.size(); ++i)
        {
            VkCommandBufferBeginInfo command_begin_info{};
            command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            VkResult begin_record_result = vkBeginCommandBuffer(command_buffers_[i], &command_begin_info);
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, begin_record_result == VK_SUCCESS, "Fail to begin record command buffer {i}");

            // Structured layout:
            // Index 0 is color
            // Index 1 is depth
            std::array<VkClearValue, 2> clear_values{};
            clear_values[0].color = {{.1f, .1f, .1f, 1.f}};
            clear_values[1].depthStencil = { 1.f, 0 };
            
            VkRenderPassBeginInfo render_pass_begin_info{};
            render_pass_begin_info.sType              = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_begin_info.renderPass         = swap_chain_->GetRenderPass();
            render_pass_begin_info.framebuffer        = swap_chain_->GetFrameBuffer(i);
            render_pass_begin_info.renderArea.offset  = {0, 0};
            render_pass_begin_info.renderArea.extent  = swap_chain_->GetSwapChainExtent();
            render_pass_begin_info.clearValueCount    = clear_values.size();
            render_pass_begin_info.pClearValues       = clear_values.data();

            vkCmdBeginRenderPass(command_buffers_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

            pipeline_->Bind(command_buffers_[i]);
            model_->Bind(command_buffers_[i]);
            model_->Draw(command_buffers_[i]);

            vkCmdEndRenderPass(command_buffers_[i]);

            VkResult end_record_result = vkEndCommandBuffer(command_buffers_[i]);
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, end_record_result == VK_SUCCESS, "Fail to end record command buffer");
        }
    }

    void KitApplication::LoadModel()
    {
        std::vector<KitModel::KitVertex> vertices
        {
            {{.0f, -.5f}, {1.f, 0.f, 0.f}},
            {{.5f, .5f}, {0.f, 1.f, 0.f}},
            {{-.5f, .5f}, {0.f, 0.f, 1.f}}
        };

        model_ = std::make_unique<KitModel>(engine_device_.get(), vertices);
    }

    void KitApplication::Draw()
    {
        uint32_t image_index;
        VkResult result = swap_chain_->AcquireNextImage(&image_index);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to aquire swapchain image index {}", image_index);

        result = swap_chain_->SubmitCommandBuffers(&command_buffers_[image_index], &image_index);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to submit command buffer");
    }
}
