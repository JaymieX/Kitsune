#include "KitWindow.h"

namespace Kitsune
{
    KitWindow::KitWindow(KitWindowInfo window_info):
        window_info_(std::move(window_info))
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window_ = glfwCreateWindow(window_info_.width, window_info_.height, window_info_.title.c_str(), nullptr, nullptr);
    }

    KitWindow::~KitWindow()
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}
