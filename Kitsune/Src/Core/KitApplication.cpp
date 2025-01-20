#include "KitApplication.h"

#include "Graphics/KitEngineDevice.h"

namespace Kitsune
{
    KitApplication::KitApplication() :
        window_(std::make_unique<KitWindow>(KitWindowInfo(default_width, default_height, default_title))),
        engine_device_(std::make_unique<KitEngineDevice>(window_.get()))
    {
    }

    KitApplication::~KitApplication()
    {
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
    }

    void KitApplication::CreatePipeline()
    {
    }

    void KitApplication::CreateCommandBuffers()
    {
    }

    void KitApplication::Draw()
    {
    }
}
