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

        LoadModel();
        CreatePipelineLayout();
        RecreateSwapChain();
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
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, swap_chain_ != nullptr, "Swap chain does not exist at pipeline creation!");
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, pipeline_layout_ != nullptr, "Pipeline layouts do not exist at pipeline creation!");
        
        PipelineConfigInfo pipeline_config{};
        KitPipeline::DefaultPipelineConfigInfo(pipeline_config);
        
        pipeline_config.render_pass     = swap_chain_->GetRenderPass();
        pipeline_config.pipeline_layout = pipeline_layout_;

        pipeline_ = std::make_unique<KitPipeline>(engine_device_.get(), "Shader/Simple3DVert.spv", "Shader/Simple3DFrag.spv", pipeline_config);
    }

    void KitApplication::RecreateSwapChain()
    {
        VkExtent2D extent = window_->GetExtent();

        while (extent.width == 0 || extent.height == 0)
        {
            extent = window_->GetExtent();
            glfwWaitEvents();
        }
        
        engine_device_->DeviceWaitIdle();

        if (swap_chain_ == nullptr)
        {
            swap_chain_ = std::make_unique<KitSwapChain>(engine_device_.get(), window_->GetExtent());
        }
        else
        {
            swap_chain_ = std::make_unique<KitSwapChain>(engine_device_.get(), window_->GetExtent(), std::move(swap_chain_));
            if (swap_chain_->ImageCount() != command_buffers_.size())
            {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }

        CreatePipeline();
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
    }

    void KitApplication::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(engine_device_->GetDevice(), engine_device_->GetCommandPool(), command_buffers_.size(), command_buffers_.data());
        command_buffers_.clear();
    }

    void KitApplication::RecordCommandBuffer(int image_index) const
    {
        VkCommandBufferBeginInfo command_begin_info{};
        command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkResult begin_record_result = vkBeginCommandBuffer(command_buffers_[image_index], &command_begin_info);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, begin_record_result == VK_SUCCESS, "Fail to begin record command buffer {}", image_index);

        // Structured layout:
        // Index 0 is color
        // Index 1 is depth
        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {{.1f, .1f, .1f, 1.f}};
        clear_values[1].depthStencil = { 1.f, 0 };
        
        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType              = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass         = swap_chain_->GetRenderPass();
        render_pass_begin_info.framebuffer        = swap_chain_->GetFrameBuffer(image_index);
        render_pass_begin_info.renderArea.offset  = {0, 0};
        render_pass_begin_info.renderArea.extent  = swap_chain_->GetSwapChainExtent();
        render_pass_begin_info.clearValueCount    = clear_values.size();
        render_pass_begin_info.pClearValues       = clear_values.data();

        vkCmdBeginRenderPass(command_buffers_[image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        // Dynamic viewport/scissor
        VkViewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(swap_chain_->GetSwapChainExtent().width);
        viewport.height   = static_cast<float>(swap_chain_->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swap_chain_->GetSwapChainExtent()};
        vkCmdSetViewport(command_buffers_[image_index], 0, 1, &viewport);
        vkCmdSetScissor(command_buffers_[image_index], 0, 1, &scissor);

        pipeline_->Bind(command_buffers_[image_index]);
        model_->Bind(command_buffers_[image_index]);
        model_->Draw(command_buffers_[image_index]);

        vkCmdEndRenderPass(command_buffers_[image_index]);

        VkResult end_record_result = vkEndCommandBuffer(command_buffers_[image_index]);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, end_record_result == VK_SUCCESS, "Fail to end record command buffer");
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

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }
        
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to aquire swapchain image index {}", image_index);

        RecordCommandBuffer(image_index);

        result = swap_chain_->SubmitCommandBuffers(&command_buffers_[image_index], &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_->HasWindowBufferResized())
        {
            window_->ResetWindowBufferResized();
            RecreateSwapChain();
            return;
        }
        
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to submit command buffer");
    }
}
