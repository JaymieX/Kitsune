#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "KitSystem.h"
#include "Core/KitLogs.h"
#include "Graphics/KitEngineDevice.h"

namespace Kitsune
{
    template <typename T>
    concept SystemConcept = std::is_base_of_v<KitSystem, T>;

    class KitSystemManager final
    {
        KitEngineDevice* device_ = nullptr;

        std::vector<std::unique_ptr<KitSystem>> systems_list_;
        std::unordered_map<std::type_index, KitSystem*> system_map_;

    public:
        void Init(KitEngineDevice* device);
        void Update(const float dt) const;
        void End();

        template <SystemConcept T>
        void AddSystem()
        {
            KIT_ASSERT(LOG_ENGINE, device_ != nullptr, "Device is null at System creation!");

            systems_list_.emplace_back(std::make_unique<T>());

            KitSystem* new_system = systems_list_.back().get();
            new_system->Init(device_);
            system_map_[typeid(T)] = new_system;
        }

        template <SystemConcept T>
        T* GetSystem()
        {
            if (!system_map_.contains(typeid(T)))
            {
                return nullptr;
            }

            return static_cast<T*>(system_map_[typeid(T)]);
        }
    };
} // Kitsune
