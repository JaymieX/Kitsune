#include "KitWindow.h"

#include "Core/KitLogs.h"

namespace Kitsune
{
    KitWindow::KitWindow(KitWindowInfo window_info):
        window_info_(std::move(window_info))
    {
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Creating window with title: {}", window_info_.title);
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window_ = glfwCreateWindow(window_info_.width, window_info_.height, window_info_.title.c_str(), nullptr, nullptr);
    }

    KitWindow::~KitWindow()
    {
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Destroying window with title: {}", window_info_.title);
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}
