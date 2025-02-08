#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Core/System/KitSystem.h"
#include "KitResourceCache.h"

namespace Kitsune
{
    template <typename T>
    concept ResourceCacheConcept = std::is_base_of_v<KitResourceCacheBase, T>;

    class KitResourceSystem final : public KitSystem
    {
        KitEngineDevice* device_ = nullptr;
        std::unordered_map<std::type_index, std::unique_ptr<KitResourceCacheBase>> resource_caches_;

    protected:
        bool Init(KitEngineDevice* device) override;
        bool End() override;

    public:
        template <ResourceCacheConcept T>
        void RegisterCache()
        {
            resource_caches_[typeid(T)] = std::make_unique<T>(device_);
        }

        template <ResourceCacheConcept T>
        T* GetCache()
        {
            if (!resource_caches_.contains(typeid(T)))
            {
                return nullptr;
            }

            return static_cast<T*>(resource_caches_[typeid(T)].get());
        }
    };
} // Kitsune
