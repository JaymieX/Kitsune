#pragma once
#include <memory>

#include "Graphics/KitPipeline.h"
#include "Graphics/KitSwapChain.h"
#include "Graphics/KitWindow.h"
#include "Core/Scene/KitGameObject.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

namespace Kitsune
{
    constexpr uint32_t default_width    = 800;
    constexpr uint32_t default_height   = 600;
    constexpr const char* default_title = "Kitsune Tools";

    struct KitPushConstantsData
    {
        glm::mat2   transform{1.f}; // Default id matrix init
        alignas(8)  glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };
    
    class KitApplication final
    {
        std::unique_ptr<KitWindow> window_;
        std::unique_ptr<KitEngineDevice> engine_device_;
        std::unique_ptr<KitSwapChain> swap_chain_;

        std::unique_ptr<KitPipeline> pipeline_;
        VkPipelineLayout pipeline_layout_;
        std::vector<VkCommandBuffer> command_buffers_;

        std::vector<KitGameObject> game_objects_;
        
    public:
        KitApplication();
        ~KitApplication();

        KitApplication(const KitApplication&) = delete;
        KitApplication(KitApplication&&)      = delete;

        KitApplication& operator=(const KitApplication&) = delete;
        KitApplication& operator=(KitApplication&&)      = delete;

        KIT_NODISCARD KitWindow* GetWindow() const { return window_.get(); }

        void Run();

    private:
        void CreatePipelineLayout();
        void CreatePipeline();
        void RecreateSwapChain();
        void RecordCommandBuffer(int image_index) const;
        void LoadGameObjects();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RenderGameObjects(VkCommandBuffer command_buffer) const;
        void Draw();
    };
}
