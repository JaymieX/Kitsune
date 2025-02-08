#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Graphics/KitEngineDevice.h"

namespace Kitsune
{
    class KitResourceCacheBase
    {
    protected:
        KitEngineDevice* device_;

    public:
        explicit KitResourceCacheBase(KitEngineDevice* device):
            device_(device)
        {
        }

        virtual ~KitResourceCacheBase() = default;
    };

    template<typename T>
    class KitResourceCache : public KitResourceCacheBase
    {
    protected:
        std::unordered_map<std::string, std::shared_ptr<T>> cache_;

    public:
        explicit KitResourceCache(KitEngineDevice* device):
            KitResourceCacheBase(device)
        {
        }

        virtual bool LoadFromFile(const std::string& name, const std::string& file_path) = 0;

        std::shared_ptr<T> Get(const std::string& name)
        {
            return cache_[name];
        }
    };
} // Kitsune
