#include "KitWindow.h"

#include "Core/KitLogs.h"

namespace Kitsune
{
    void KitWindow::FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        KitWindow* kit_window = reinterpret_cast<KitWindow*>(glfwGetWindowUserPointer(window));
        
        kit_window->has_frame_buffer_resized_ = true;
        kit_window->window_info_.width        = width;
        kit_window->window_info_.height       = height;
    }

    KitWindow::KitWindow(KitWindowInfo window_info):
        window_info_(std::move(window_info))
    {
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Creating window with title: {}", window_info_.title);
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window_ = glfwCreateWindow(window_info_.width, window_info_.height, window_info_.title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window_, this);
        glfwSetWindowSizeCallback(window_, FrameBufferSizeCallback);
    }

    KitWindow::~KitWindow()
    {
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Destroying window with title: {}", window_info_.title);
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}
