#include "KitApplication.h"

#include "KitLogs.h"
#include "Graphics/KitEngineDevice.h"

namespace Kitsune
{
    KitApplication::KitApplication()
    {
        KitLog::InitLoggers();
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application starting...");

        window_ = std::make_unique<KitWindow>(KitWindowInfo(default_width, default_height, default_title));
        engine_device_ = std::make_unique<KitEngineDevice>(window_.get());
        swap_chain_ = std::make_unique<KitSwapChain>(engine_device_.get());
    }

    KitApplication::~KitApplication()
    {
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
