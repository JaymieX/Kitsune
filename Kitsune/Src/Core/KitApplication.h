#pragma once
#include <memory>

#include "Graphics/KitModel.h"
#include "Graphics/KitPipeline.h"
#include "Graphics/KitSwapChain.h"
#include "Graphics/KitWindow.h"

namespace Kitsune
{
    constexpr uint32_t default_width    = 800;
    constexpr uint32_t default_height   = 600;
    constexpr const char* default_title = "Kitsune Tools";
    
    class KitApplication final
    {
        std::unique_ptr<KitWindow> window_;
        std::unique_ptr<KitEngineDevice> engine_device_;
        std::unique_ptr<KitSwapChain> swap_chain_;

        std::unique_ptr<KitPipeline> pipeline_;
        VkPipelineLayout pipeline_layout_;
        std::vector<VkCommandBuffer> command_buffers_;

        std::unique_ptr<KitModel> model_;
        
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
        void LoadModel();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void Draw();
    };
}
