#pragma once
#include <memory>
#include <vector>

#include "KitRenderSystemBase.h"

namespace Kitsune
{
    template <typename T> concept RenderSystemConcept = std::is_base_of_v<KitRenderSystemBase, T>;

    class KitRenderSystemManager
    {
        KitEngineDevice* engine_device_;
        std::vector<std::unique_ptr<KitRenderSystemBase>> render_systems_;

    public:
        explicit KitRenderSystemManager(KitEngineDevice* device):
            engine_device_(device)
        {
        }

        template<RenderSystemConcept T>
        void RegisterRenderSystem()
        {
            render_systems_.emplace_back(std::make_unique<T>(engine_device_));
        }

        void Init(
            const VkRenderPass          render_pass,
            const VkDescriptorSetLayout descriptor_set_layout) const
        {
            for (const auto& system : render_systems_)
            {
                system->Init(render_pass, descriptor_set_layout);
            }
        }

        void Update(const KitFrameInfo &frame_info, KitGlobalUBO &ubo) const
        {
            for (const auto& system : render_systems_)
            {
                system->Update(frame_info, ubo);
            }
        }

        void Render(const KitFrameInfo& frame_info) const
        {
            for (const auto& system : render_systems_)
            {
                system->Render(frame_info);
            }
        }
    };
}
