#pragma once
#include <string>
#include <utility>

#include "Graphics/KitEngineDevice.h"

namespace Kitsune
{
    class KitSystem
    {
        friend class KitSystemManager;

    protected:
        std::string system_name_;

        virtual bool Init(KitEngineDevice* device) = 0;
        virtual void Update(const float dt) {};
        virtual bool End() = 0;

    public:
        virtual ~KitSystem() = default;
    };

} // Kitsune

