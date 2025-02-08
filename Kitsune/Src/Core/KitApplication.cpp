#include "KitApplication.h"

#include "KitLogs.h"
#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitModel.h"
#include "Graphics/KitPipeline.h"
#include "Graphics/RenderSystems/KitBasicRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <chrono>

#include "KitInputController.h"
#include "System/Subsystems/KitResourceSystem.h"
#include "System/Subsystems/Caches/KitModelResourceCache.h"

namespace Kitsune
{
    KitApplication::KitApplication()
    {
        KitLog::InitLoggers();
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application starting...");

        window_ = std::make_unique<KitWindow>(KitWindowInfo(default_width, default_height, default_title));
        engine_device_ = std::make_unique<KitEngineDevice>(window_.get());
        renderer_ = std::make_unique<KitRenderer>(window_.get(), engine_device_.get());

        system_manager_.Init(engine_device_.get());
        system_manager_.AddSystem<KitResourceSystem>();

        LoadGameObjects();
    }

    KitApplication::~KitApplication()
    {
        system_manager_.End();

        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application closing...");
    }

    void KitApplication::Run()
    {
        KitBasicRenderSystem basic_render_system(engine_device_.get(), renderer_->GetRenderPass());
        KitCamera camera;
        //camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, .5f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewer_object = KitGameObject::CreateGameObject();
        KitInputController input_controller;

        auto start = std::chrono::high_resolution_clock::now();
        
        while (!window_->ShouldClose())
        {
            glfwPollEvents();

            auto now = std::chrono::high_resolution_clock::now();
            float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(now - start).count();
            start = now;

            system_manager_.Update(frame_time);

            input_controller.MoveXZ(window_->window_, frame_time, viewer_object);
            camera.SetViewYXZ(viewer_object.transform.translation, viewer_object.transform.rotation);
            
            float aspect = renderer_->GetAspectRatio();
            //camera.SetOrthographicProjectionMatrix(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjectionMatrix(glm::radians(50.f), aspect, 0.1f, 10.f);
            
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
      KitMeshData modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
       
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
       
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
       
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
       
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
       
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
          v.position += offset;
        }
       
        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

        std::unique_ptr<KitModel> result = std::make_unique<KitModel>();
        result->AddMesh(device, modelBuilder);
        return result;
    }

    std::shared_ptr<KitModel> KitApplication::LoadModel()
    {
        KitResourceSystem* resource_system = system_manager_.GetSystem<KitResourceSystem>();
        resource_system->RegisterCache<KitModelResourceCache>();
        KitModelResourceCache* model_resource = resource_system->GetCache<KitModelResourceCache>();

        model_resource->LoadFromFile("pot", "C:/Users/jaymi/OneDrive/Desktop/colored_cube.obj");

        return model_resource->Get("pot");
    }

    void KitApplication::LoadGameObjects()
    {
        std::shared_ptr<KitModel> cube_model = createCubeModel(engine_device_.get(), glm::vec3(0.0f));
        std::shared_ptr<KitModel> vase_model = LoadModel();

        auto cube_go = KitGameObject::CreateGameObject();
        cube_go.model = vase_model;
        cube_go.transform.translation = {.0f, .0f, 2.5f};
        cube_go.transform.scale       = {.5f, .5f, .5f};
        
        game_objects_.push_back(cube_go);
    }
}
