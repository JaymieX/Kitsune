#include "KitApplication.h"

#include "KitLogs.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitModel.h"
#include "Graphics/KitPipeline.h"
#include "Graphics/RenderSystems/BasicRenderSystem.h"

namespace Kitsune
{
    KitApplication::KitApplication()
    {
        KitLog::InitLoggers();
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application starting...");

        window_        = std::make_unique<KitWindow>(KitWindowInfo(default_width, default_height, default_title));
        engine_device_ = std::make_unique<KitEngineDevice>(window_.get());
        renderer_      = std::make_unique<KitRenderer>(window_.get(), engine_device_.get());

        LoadGameObjects();
    }

    KitApplication::~KitApplication()
    {
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application closing...");
    }

    void KitApplication::Run()
    {
        BasicRenderSystem basic_render_system(engine_device_.get(), renderer_->GetRenderPass());
        
        while (!window_->ShouldClose())
        {
            glfwPollEvents();
            
            if (VkCommandBuffer command_buffer = renderer_->BeginFrame())
            {
                renderer_->BeginSwapChainRenderPass(command_buffer);
                basic_render_system.RenderGameObjects(command_buffer, game_objects_);
                renderer_->EndSwapChainRenderPass(command_buffer);

                renderer_->EndFrame();
            }
        }

        engine_device_->DeviceWaitIdle();
    }

    void KitApplication::LoadGameObjects()
    {
        std::vector<KitModel::KitVertex> vertices
        {
            {{.0f, -.5f}, {1.f, 0.f, 0.f}},
            {{.5f, .5f}, {0.f, 1.f, 0.f}},
            {{-.5f, .5f}, {0.f, 0.f, 1.f}}
        };

        const std::shared_ptr<KitModel> model = std::make_shared<KitModel>(engine_device_.get(), vertices);

        auto triangle = KitGameObject::CreateGameObject();
        triangle.model = model;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};

        game_objects_.push_back(triangle);
    }
}
