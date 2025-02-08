#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#include "KitEngineDevice.h"

namespace Kitsune
{
    struct KitVertex
    {
        glm::vec3 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription>   GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    struct KitMeshData
    {
        std::vector<KitVertex> vertices;
        std::vector<uint32_t>  indices;
    };

    class KitMesh
    {
        KitEngineDevice* device_;

        bool is_index_available = false;

        VkBuffer vertex_buffer_;
        VkDeviceMemory vertex_buffer_memory_;
        uint32_t vertex_count_;

        VkBuffer index_buffer_;
        VkDeviceMemory index_buffer_memory_;
        uint32_t index_count_;

        bool is_moved_ = false;
        
    public:
        KitMesh(KitEngineDevice* device, const KitMeshData& data);
        ~KitMesh();

        KitMesh(KitMesh&& other);
        KitMesh& operator=(KitMesh&& other);

        void Bind(VkCommandBuffer command_buffer) const;
        void Draw(VkCommandBuffer command_buffer) const;

    private:
        void CreateVertexBuffers(const std::vector<KitVertex>& vertices);
        void CreateIndexBuffers(const std::vector<uint32_t>& indices);
    };

    class KitModel
    {
        std::vector<KitMesh> meshes_;

    public:
        KitModel() = default;
        explicit KitModel(std::vector<KitMesh>&& meshes);
        ~KitModel();

        void AddMesh(KitEngineDevice* device, const KitMeshData& data);

        void Bind(VkCommandBuffer command_buffer) const;
        void Draw(VkCommandBuffer command_buffer) const;
    };
}
