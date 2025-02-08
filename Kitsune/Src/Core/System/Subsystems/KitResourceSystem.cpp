#include "KitResourceSystem.h"

namespace Kitsune
{
    bool KitResourceSystem::Init(KitEngineDevice* device)
    {
        device_ = device;
        return true;
    }

    bool KitResourceSystem::End()
    {
        return true;
    }
} // Kitsune