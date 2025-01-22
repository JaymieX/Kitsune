#include "KitApplication.h"

#include "KitLogs.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitModel.h"
#include "Graphics/KitPipeline.h"
#include "Graphics/RenderSystems/KitBasicRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

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
        KitBasicRenderSystem basic_render_system(engine_device_.get(), renderer_->GetRenderPass());
        KitCamera camera;
        
        while (!window_->ShouldClose())
        {
            glfwPollEvents();
            float aspect = renderer_->GetAspectRatio();
            //camera.SetOrthographicProjectionMatrix(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjectionMatrix(glm::radians(45.f), aspect, 0.1f, 50.f);
            
            if (VkCommandBuffer command_buffer = renderer_->BeginFrame())
            {
                renderer_->BeginSwapChainRenderPass(command_buffer);
                basic_render_system.RenderGameObjects(command_buffer, game_objects_, camera);
                renderer_->EndSwapChainRenderPass(command_buffer);

                renderer_->EndFrame();
            }
        }

        engine_device_->DeviceWaitIdle();
    }

    std::unique_ptr<KitModel> createCubeModel(KitEngineDevice* device, glm::vec3 offset) {
      std::vector<KitModel::KitVertex> vertices{
     
          // left face (white)
          {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
          {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
          {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
          {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
          {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
          {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
     
          // right face (yellow)
          {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
          {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
          {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
          {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
     
          // top face (orange, remember y axis points down)
          {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
          {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
          {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
          {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
          {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
          {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
     
          // bottom face (red)
          {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
          {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
          {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
          {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
     
          // nose face (blue)
          {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
          {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
          {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
          {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
          {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
          {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
     
          // tail face (green)
          {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
          {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
          {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
     
      };
      for (auto& v : vertices) {
        v.position += offset;
      }
      return std::make_unique<KitModel>(device, vertices);
    }

    void KitApplication::LoadGameObjects()
    {
        std::shared_ptr<KitModel> cube_model = createCubeModel(engine_device_.get(), glm::vec3(0.0f));
        auto cube_go = KitGameObject::CreateGameObject();
        cube_go.model = cube_model;
        cube_go.transform.translation = {.0f, .0f, -2.5f};
        cube_go.transform.scale       = {.5f, .5f, .5f};
        
        game_objects_.push_back(cube_go);
    }
}
