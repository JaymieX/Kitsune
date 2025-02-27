#pragma once
#include <memory>

#include "Graphics/KitWindow.h"
#include "Core/Scene/KitGameObject.h"
#include "Graphics/KitDescriptor.h"

#include "Graphics/KitRenderer.h"
#include "System/KitSystemManager.h"

namespace Kitsune
{
    constexpr uint32_t default_width    = 800;
    constexpr uint32_t default_height   = 600;
    constexpr const char* default_title = "Kitsune Tools";
    
    class KitApplication final
    {
        std::unique_ptr<KitWindow> window_;
        std::unique_ptr<KitEngineDevice> engine_device_;
        std::unique_ptr<KitRenderer> renderer_;

        KitSystemManager system_manager_;

        std::unique_ptr<KitDescriptorPool> descriptor_pool_;
        std::vector<KitGameObject> game_objects_;

        std::shared_ptr<KitModel> quad_model_ = nullptr;
        std::shared_ptr<KitModel> vase_model_ = nullptr;
        
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
        void LoadModel();
        void LoadGameObjects();
    };
}
