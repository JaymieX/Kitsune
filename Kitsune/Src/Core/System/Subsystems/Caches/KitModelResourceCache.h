#pragma once
#include <assimp/scene.h>


#include "Core/System/Subsystems/KitResourceCache.h"
#include "Graphics/KitModel.h"

namespace Kitsune
{
    class KitModelResourceCache final : public KitResourceCache<KitModel>
    {
        void ProcessNode(aiNode* node, const aiScene* scene, KitModel* model);

    public:
        explicit KitModelResourceCache(KitEngineDevice* device):
            KitResourceCache<KitModel>(device)
        {
        }

        bool LoadFromFile(const std::string& name, const std::string& file_path) override;
    };
} // Kitsune
