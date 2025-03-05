#include "KitApplication.h"

#include "Graphics/KitEngineDevice.h"
#include "Graphics/KitModel.h"
#include "Graphics/KitPipeline.h"
#include "Graphics/RenderSystems/KitBasicRenderSystem.h"
#include "KitLogs.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <chrono>

#include "Graphics/KitGlobalGraphicsDefines.h"
#include "KitInputController.h"
#include "Graphics/RenderSystems/KitGizmoBillboardRenderSystem.h"
#include "System/Subsystems/Caches/KitModelResourceCache.h"
#include "System/Subsystems/KitResourceSystem.h"

namespace Kitsune
{
    KitApplication::KitApplication()
    {
        KitLog::InitLoggers();
        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application starting...");

        window_        = std::make_unique<KitWindow>(KitWindowInfo(default_width, default_height, default_title));
        engine_device_ = std::make_unique<KitEngineDevice>(window_.get());
        renderer_      = std::make_unique<KitRenderer>(window_.get(), engine_device_.get());

        system_manager_.Init(engine_device_.get());
        system_manager_.AddSystem<KitResourceSystem>();

        descriptor_pool_ = KitDescriptorPool::KitDescriptorPoolBuilder(engine_device_.get())
                           .SetMaxSets(KitSwapChain::MAX_FRAMES_IN_FLIGHT)
                           .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, KitSwapChain::MAX_FRAMES_IN_FLIGHT)
                           .Build();

        LoadGameObjects();
    }

    KitApplication::~KitApplication()
    {
        system_manager_.End();

        KIT_LOG(LOG_ENGINE, Kitsune::KitLogLevel::LOG_INFO, "Application closing...");
    }

    void KitApplication::Run()
    {
        std::vector<std::unique_ptr<KitGraphicsBuffer>> ubo_buffers(KitSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < ubo_buffers.size(); i++)
        {
            ubo_buffers[i] = std::make_unique<KitGraphicsBuffer>(
                engine_device_.get(),
                sizeof(KitGlobalUBO),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            ubo_buffers[i]->Map();
        }

        auto global_set_layout = KitDescriptorSetLayout::KitDescriptorSetLayoutBuilder(engine_device_.get())
                                 .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL)
                                 .Build();

        std::vector<VkDescriptorSet> global_descriptor_sets(KitSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < KitSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
        {
            auto buffer_info = ubo_buffers[i]->DescriptorInfo();
            KitDescriptorWriter(*global_set_layout, *descriptor_pool_)
                .WriteBuffer(0, &buffer_info)
                .Build(global_descriptor_sets[i]);
        }

        KitBasicRenderSystem basic_render_system(
            engine_device_.get(),
            renderer_->GetRenderPass(),
            global_set_layout->GetDescriptorSetLayout());

        KitGizmoBillboardRenderSystem billboard_render_system(
            engine_device_.get(),
            renderer_->GetRenderPass(),
            global_set_layout->GetDescriptorSetLayout());

        KitCamera camera;
        // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, .5f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto               viewer_object = KitGameObject::CreateGameObject();
        KitInputController input_controller;

        auto start = std::chrono::high_resolution_clock::now();

        while (!window_->ShouldClose())
        {
            glfwPollEvents();

            auto  now        = std::chrono::high_resolution_clock::now();
            float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(now - start).count();
            start            = now;

            system_manager_.Update(frame_time);

            input_controller.MoveXZ(window_->window_, frame_time, viewer_object);
            camera.SetViewYXZ(viewer_object.transform.translation, viewer_object.transform.rotation);

            float aspect = renderer_->GetAspectRatio();
            // camera.SetOrthographicProjectionMatrix(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjectionMatrix(glm::radians(50.f), aspect, 0.1f, 10.f);

            if (VkCommandBuffer command_buffer = renderer_->BeginFrame())
            {
                int          frame_index = renderer_->GetCurrentFrameIndex();
                KitFrameInfo frame_info{frame_index, frame_time, command_buffer, &camera, global_descriptor_sets[frame_index],
                                        game_objects_};

                // Update
                KitGlobalUBO global_ubo;
                global_ubo.projection = camera.GetProjectionMatrix();
                global_ubo.view       = camera.GetViewMatrix();
                billboard_render_system.Update(frame_info, global_ubo);
                ubo_buffers[frame_index]->WriteToBuffer(&global_ubo);
                ubo_buffers[frame_index]->Flush(); // Manual flush because we didn't use host coherent

                // Render
                renderer_->BeginSwapChainRenderPass(command_buffer);
                basic_render_system.RenderGameObjects(frame_info, game_objects_);
                billboard_render_system.RenderGameObjects(frame_info, game_objects_);
                renderer_->EndSwapChainRenderPass(command_buffer);

                renderer_->EndFrame();
            }
        }

        engine_device_->DeviceWaitIdle();
    }

    void KitApplication::LoadModel()
    {
    }

    void KitApplication::LoadGameObjects()
    {
        KitResourceSystem* resource_system = system_manager_.GetSystem<KitResourceSystem>();
        resource_system->RegisterCache<KitModelResourceCache>();
        KitModelResourceCache* model_resource = resource_system->GetCache<KitModelResourceCache>();

        model_resource->LoadFromFile("quad", "Resources/quad.obj");
        quad_model_ = model_resource->Get("quad");

        model_resource->LoadFromFile("pot", "C:/Users/jaymi/OneDrive/Desktop/smooth_vase.obj");
        vase_model_ = model_resource->Get("pot");

        auto vase_go            = KitGameObject::CreateGameObject();
        vase_go.model           = vase_model_;
        vase_go.transform.scale = {2.5f, 2.5f, 2.5f};

        auto quad_go            = KitGameObject::CreateGameObject();
        quad_go.model           = quad_model_;
        quad_go.transform.scale = {2.5f, 2.5f, 2.5f};

        game_objects_.push_back(vase_go);
        game_objects_.push_back(quad_go);

        std::vector<glm::vec3> lightColors{
          {1.f, .1f, .1f},
          {.1f, .1f, 1.f},
          {.1f, 1.f, .1f},
          {1.f, 1.f, .1f},
          {.1f, 1.f, 1.f},
          {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++)
        {
            auto point_light  = KitGameObject::CreatePointLight(0.2f);
            point_light.color = lightColors[i];
            auto rotate_light = glm::rotate(
                glm::mat4(1.f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.f, -1.f, 0.f});
            point_light.transform.translation = glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            game_objects_.push_back(point_light);
        }
    }
} // namespace Kitsune
