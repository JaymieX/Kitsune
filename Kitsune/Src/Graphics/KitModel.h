#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#include "KitEngineDevice.h"

namespace Kitsune
{
    class KitModel
    {
        KitEngineDevice* device_;

        VkBuffer vertex_buffer_;
        VkDeviceMemory vertex_buffer_memory_;
        uint32_t vertex_count_;
        
    public:
        struct KitVertex
        {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription>   GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        KitModel(KitEngineDevice* device, const std::vector<KitVertex>& vertices);
        ~KitModel();

        void Bind(VkCommandBuffer command_buffer) const;
        void Draw(VkCommandBuffer command_buffer) const;

    private:
        void CreateVertexBuffers(const std::vector<KitVertex>& vertices);
    };
}
