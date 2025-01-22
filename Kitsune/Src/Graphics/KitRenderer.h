#pragma once
#include <memory>

#include "KitSwapChain.h"
#include "KitWindow.h"
#include "Core/KitLogs.h"

namespace Kitsune
{
    class KitRenderer
    {
        KitWindow* window_;
        KitEngineDevice* engine_device_;

        std::unique_ptr<KitSwapChain> swap_chain_;

        std::vector<VkCommandBuffer> command_buffers_;

        uint32_t current_image_index_;
        int      current_frame_index_;

        bool has_frame_started_ = false;
        
    public:
        KitRenderer(KitWindow* window, KitEngineDevice* engine_device);
        ~KitRenderer();

        KitRenderer(const KitRenderer&) = delete;
        KitRenderer(KitRenderer&&)      = delete;
        
        KitRenderer& operator=(const KitRenderer&) = delete;
        KitRenderer& operator=(KitRenderer&&) = delete;

        KIT_NODISCARD bool IsFrameInProgress() const { return has_frame_started_; }
        KIT_NODISCARD VkRenderPass GetRenderPass() const { return swap_chain_->GetRenderPass(); }

        KIT_NODISCARD int GetCurrentFrameIndex() const
        {
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, has_frame_started_, "Frame index not accessible when frame is not in progress!");
            return current_frame_index_;
        }
        
        KIT_NODISCARD VkCommandBuffer GetCurrentCommandBuffer() const
        {
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, has_frame_started_, "Command buffer not accessible when frame is not in progress!");
            return command_buffers_[current_frame_index_];
        }
        
        VkCommandBuffer BeginFrame();
        void EndFrame();

        void BeginSwapChainRenderPass(VkCommandBuffer command_buffer) const;
        void EndSwapChainRenderPass(VkCommandBuffer command_buffer) const;

    private:
        void RecreateSwapChain();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
    };
}
