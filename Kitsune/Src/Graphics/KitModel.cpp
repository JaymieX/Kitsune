#include "KitModel.h"

#include "Core/KitLogs.h"

namespace Kitsune
{
    std::vector<VkVertexInputBindingDescription> KitModel::KitVertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
        binding_descriptions[0].binding   = 0;
        binding_descriptions[0].stride    = sizeof(KitVertex);
        binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_descriptions;
    }

    std::vector<VkVertexInputAttributeDescription> KitModel::KitVertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
        // Position
        attribute_descriptions[0].binding  = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format   = VK_FORMAT_R32G32_SFLOAT; // vec2
        attribute_descriptions[0].offset   = offsetof(KitVertex, position);

        // Color
        attribute_descriptions[1].binding  = 0;
        attribute_descriptions[1].location = 1; // Loc used in ver shader
        attribute_descriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT; // vec3
        attribute_descriptions[1].offset   = offsetof(KitVertex, color);
        
        return attribute_descriptions;
    }

    KitModel::KitModel(KitEngineDevice* device, const std::vector<KitVertex>& vertices):
        device_(device)
    {
        CreateVertexBuffers(vertices);
    }

    KitModel::~KitModel()
    {
        vkFreeMemory(device_->GetDevice(), vertex_buffer_memory_, nullptr);
        vkDestroyBuffer(device_->GetDevice(), vertex_buffer_, nullptr);
    }

    void KitModel::Bind(VkCommandBuffer command_buffer) const
    {
        const VkBuffer bufffers[] = { vertex_buffer_ };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(command_buffer, 0, 1, bufffers, offsets);
    }

    void KitModel::Draw(VkCommandBuffer command_buffer) const
    {
        vkCmdDraw(command_buffer, vertex_count_, 1, 0, 0);
    }

    void KitModel::CreateVertexBuffers(const std::vector<KitVertex>& vertices)
    {
        vertex_count_ = static_cast<uint32_t>(vertices.size());

        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, vertex_count_ >= 3, "Vertex count must be at least 3!");
        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;

        device_->CreateBuffer(
            buffer_size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // CPU acess
            vertex_buffer_,
            vertex_buffer_memory_);

        void* data;
        vkMapMemory(device_->GetDevice(), vertex_buffer_memory_, 0, buffer_size, 0, &data); // point to beginning
        memcpy(data, vertices.data(), static_cast<size_t>(buffer_size)); // copy to host map
        vkUnmapMemory(device_->GetDevice(), vertex_buffer_memory_);
    }
}
