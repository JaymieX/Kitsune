#include "KitSystemManager.h"

namespace Kitsune
{
    void KitSystemManager::Init(KitEngineDevice* device)
    {
        KIT_LOG(LOG_ENGINE, KitLogLevel::LOG_INFO, "Systems initializing...");
        device_ = device;
    }

    void KitSystemManager::Update(const float dt) const
    {
        for (auto&& system : systems_list_)
        {
            system->Update(dt);
        }
    }

    void KitSystemManager::End()
    {
        for (auto&& system : systems_list_)
        {
            system->End();
        }

        systems_list_.clear();
        system_map_.clear();
    }
} // Kitsune