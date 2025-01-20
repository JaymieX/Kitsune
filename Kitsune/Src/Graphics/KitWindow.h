#pragma once

#include <string>
#include <GLFW/glfw3.h>

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
        
        KitWindowInfo window_info_;
        GLFWwindow* window_ = nullptr;
        
    public:
        explicit KitWindow(KitWindowInfo window_info);
        ~KitWindow();

        KitWindow(const KitWindow&) = delete;
        KitWindow(KitWindow&&)      = delete;
        
        KitWindow& operator=(const KitWindow&) = delete;
        KitWindow& operator=(KitWindow&&)      = delete;

        KIT_NODISCARD bool ShouldClose() const { return glfwWindowShouldClose(window_); }

        void GetFrameBufferSize(int& width, int& height) const { glfwGetFramebufferSize(window_, &width, &height); }
    };
}
