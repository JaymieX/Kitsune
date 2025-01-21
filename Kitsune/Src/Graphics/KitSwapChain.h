#pragma once
#include "KitEngineDevice.h"

namespace Kitsune
{
    class KitSwapChain
    {
        KitEngineDevice* device_;

        VkExtent2D swap_chain_extent_;
        VkFormat swap_chain_image_format_;

        VkSwapchainKHR swap_chain_;

        std::vector<VkImage> swap_chain_images_;
        std::vector<VkImageView> swap_chain_image_views_;
        std::vector<VkFramebuffer> swap_chain_framebuffers_;

        VkRenderPass render_pass_;

        std::vector<VkImage> depth_images_;
        
        std::vector<VkDeviceMemory> depth_image_memories_;
        std::vector<VkImageView> depth_image_views_;

        std::vector<VkSemaphore> image_available_semaphores_;
        std::vector<VkSemaphore> render_finished_semaphores_;
        std::vector<VkFence> in_flight_fences_;
        std::vector<VkFence> images_in_flight_;
        size_t current_frame_ = 0;
        
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        
        explicit KitSwapChain(KitEngineDevice* device);
        ~KitSwapChain();

        KitSwapChain(const KitSwapChain&) = delete;
        KitSwapChain(KitSwapChain&&)      = delete;
        
        KitSwapChain& operator=(const KitSwapChain&) = delete;
        KitSwapChain& operator=(KitSwapChain&&)      = delete;

        KIT_NODISCARD VkFramebuffer GetFrameBuffer(int index) const { return swap_chain_framebuffers_[index]; }
        KIT_NODISCARD VkRenderPass GetRenderPass() const            { return render_pass_; }
        KIT_NODISCARD VkImageView GetImageView(int index) const     { return swap_chain_image_views_[index]; }
        KIT_NODISCARD size_t ImageCount() const                     { return swap_chain_images_.size(); }
        KIT_NODISCARD VkFormat GetSwapChainImageFormat() const      { return swap_chain_image_format_; }
        KIT_NODISCARD VkExtent2D GetSwapChainExtent() const         { return swap_chain_extent_; }
        KIT_NODISCARD uint32_t Width() const                        { return swap_chain_extent_.width; }
        KIT_NODISCARD uint32_t Height() const                       { return swap_chain_extent_.height; }

        KIT_NODISCARD float ExtentAspectRatio() const
        {
            return static_cast<float>(swap_chain_extent_.width) / static_cast<float>(swap_chain_extent_.height);
        }

        VkResult AcquireNextImage(uint32_t* image_index) const;
        VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, const uint32_t* image_index);

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
        VkFormat FindDepthFormat() const;
    };
}
