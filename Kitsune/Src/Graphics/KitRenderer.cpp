#include "KitRenderer.h"

namespace Kitsune
{
    KitRenderer::KitRenderer(KitWindow* window, KitEngineDevice* engine_device):
        window_(window),
        engine_device_(engine_device)
    {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    KitRenderer::~KitRenderer()
    {
        FreeCommandBuffers();
    }

    VkCommandBuffer KitRenderer::BeginFrame()
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, !has_frame_started_, "BeginFrame() executed while a frame is already in progress!");
        
        VkResult result = swap_chain_->AcquireNextImage(&current_image_index_);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return nullptr;
        }
        
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to aquire swapchain image index {}", current_image_index_);

        has_frame_started_ = true;
        
        VkCommandBuffer command_buffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo command_begin_info{};
        command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkResult begin_record_result = vkBeginCommandBuffer(command_buffer, &command_begin_info);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, begin_record_result == VK_SUCCESS, "Fail to begin record command buffer {}", current_image_index_);

        return command_buffer;
    }

    void KitRenderer::EndFrame()
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, has_frame_started_, "EndFrame() executed while a frame is not in progress!");

        VkCommandBuffer command_buffer = GetCurrentCommandBuffer();

        VkResult result = vkEndCommandBuffer(command_buffer);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to end record command buffer");

        result = swap_chain_->SubmitCommandBuffers(&command_buffer, &current_image_index_);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_->HasWindowBufferResized())
        {
            window_->ResetWindowBufferResized();
            RecreateSwapChain();
        }
        else
        {
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to submit command buffer");
        }

        has_frame_started_ = false;
        current_frame_index_ = (current_frame_index_ + 1) % KitSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void KitRenderer::BeginSwapChainRenderPass(VkCommandBuffer command_buffer) const
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, has_frame_started_, "BeginSwapChainRenderPass() executed while a frame is not in progress!");
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, command_buffer == GetCurrentCommandBuffer(), "BeginSwapChainRenderPass() executed with a different command buffer!");

        // Structured layout:
        // Index 0 is color
        // Index 1 is depth
        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {{.01f, .01f, .01f, 1.f}};
        clear_values[1].depthStencil = { 1.f, 0 };
        
        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType              = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass         = swap_chain_->GetRenderPass();
        render_pass_begin_info.framebuffer        = swap_chain_->GetFrameBuffer(current_image_index_);
        render_pass_begin_info.renderArea.offset  = {0, 0};
        render_pass_begin_info.renderArea.extent  = swap_chain_->GetSwapChainExtent();
        render_pass_begin_info.clearValueCount    = clear_values.size();
        render_pass_begin_info.pClearValues       = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        // Dynamic viewport/scissor
        VkViewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(swap_chain_->GetSwapChainExtent().width);
        viewport.height   = static_cast<float>(swap_chain_->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swap_chain_->GetSwapChainExtent()};
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    }

    void KitRenderer::EndSwapChainRenderPass(VkCommandBuffer command_buffer) const
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, has_frame_started_, "EndSwapChainRenderPass() executed while a frame is not in progress!");
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, command_buffer == GetCurrentCommandBuffer(), "EndSwapChainRenderPass() executed with a different command buffer!");

        vkCmdEndRenderPass(command_buffer);
    }

    void KitRenderer::RecreateSwapChain()
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
            swap_chain_ = std::make_unique<KitSwapChain>(engine_device_, window_->GetExtent());
        }
        else
        {
            std::shared_ptr<KitSwapChain> old_swap_chain = std::move(swap_chain_);
            swap_chain_ = std::make_unique<KitSwapChain>(engine_device_, window_->GetExtent(), old_swap_chain);

            if (!old_swap_chain->CompareSwapFormats(*swap_chain_))
            {
                KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, KitLogLevel::LOG_INFO, "Swap chain format was changed");
            }
        }
    }

    void KitRenderer::CreateCommandBuffers()
    {
        command_buffers_.resize(KitSwapChain::MAX_FRAMES_IN_FLIGHT);
        KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, KitLogLevel::LOG_INFO, "Number of command buffers to create: {}", command_buffers_.size());

        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandPool        = engine_device_->GetCommandPool();
        allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

        VkResult result = vkAllocateCommandBuffers(engine_device_->GetDevice(), &allocate_info, command_buffers_.data());
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create command buffers!");
    }

    void KitRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(engine_device_->GetDevice(), engine_device_->GetCommandPool(), command_buffers_.size(), command_buffers_.data());
        command_buffers_.clear();
    }
}
