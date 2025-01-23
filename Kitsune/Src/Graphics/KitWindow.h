#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "Core/KitDefinitions.h"

#define GLFW_INCLUDE_VULKAN

namespace Kitsune
{
    struct KitWindowInfo
    {
        uint32_t width;
        uint32_t height;

        std::string title;
    };
    
    class KitWindow
    {
        friend class KitEngineDevice;
        friend class KitApplication;
        
        KitWindowInfo window_info_;
        bool has_frame_buffer_resized_ = false;
        
        GLFWwindow* window_ = nullptr;

        static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
        
    public:
        explicit KitWindow(KitWindowInfo window_info);
        ~KitWindow();

        KitWindow(const KitWindow&) = delete;
        KitWindow(KitWindow&&)      = delete;
        
        KitWindow& operator=(const KitWindow&) = delete;
        KitWindow& operator=(KitWindow&&)      = delete;

        KIT_NODISCARD bool ShouldClose() const { return glfwWindowShouldClose(window_); }

        bool HasWindowBufferResized() const { return has_frame_buffer_resized_; }
        
        void GetFrameBufferSize(int& width, int& height) const { glfwGetFramebufferSize(window_, &width, &height); }
        KIT_NODISCARD VkExtent2D GetExtent() const { return {static_cast<uint32_t>(window_info_.width), static_cast<uint32_t>(window_info_.height)}; }

        void ResetWindowBufferResized() { has_frame_buffer_resized_ = false; }
    };
}
