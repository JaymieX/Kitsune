#include "KitModel.h"

#include "Core/KitLogs.h"

namespace Kitsune
{
    std::vector<VkVertexInputBindingDescription> KitVertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
        binding_descriptions[0].binding   = 0;
        binding_descriptions[0].stride    = sizeof(KitVertex);
        binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_descriptions;
    }

    // Update whenever vertex struct was changed
    std::vector<VkVertexInputAttributeDescription> KitVertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
        // Position
        attribute_descriptions.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(KitVertex, position));

        // Color
        attribute_descriptions.emplace_back(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(KitVertex, color));

        // Normal
        attribute_descriptions.emplace_back(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(KitVertex, normal));

        // UV
        attribute_descriptions.emplace_back(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(KitVertex, uv));

        return attribute_descriptions;
    }

    KitMesh::KitMesh(KitEngineDevice* device, const KitMeshData& data) :
        device_(device)
    {
        CreateVertexBuffers(data.vertices);
        CreateIndexBuffers(data.indices);
    }

    KitMesh::~KitMesh()
    {
        if (is_moved_)
        {
            return;
        }
    }

    KitMesh::KitMesh(KitMesh&& other) :
        device_(other.device_),
        vertex_buffer_(std::move(other.vertex_buffer_)),
        vertex_count_(other.vertex_count_),
        index_buffer_(std::move(other.index_buffer_)),
        index_count_(other.index_count_)
    {
        other.device_        = nullptr;
        other.vertex_buffer_ = nullptr;
        other.vertex_count_  = 0;
        other.index_buffer_  = nullptr;
        other.index_count_   = 0;

        other.is_moved_ = true;
    }

    KitMesh& KitMesh::operator=(KitMesh&& other)
    {
        device_        = other.device_;
        vertex_buffer_ = std::move(other.vertex_buffer_);
        vertex_count_  = other.vertex_count_;
        index_buffer_  = std::move(other.index_buffer_);
        index_count_   = other.index_count_;

        other.device_        = nullptr;
        other.vertex_buffer_ = nullptr;
        other.vertex_count_  = 0;
        other.index_buffer_  = nullptr;
        other.index_count_   = 0;

        other.is_moved_ = true;

        return *this;
    }

    void KitMesh::Bind(VkCommandBuffer command_buffer) const
    {
        const VkBuffer bufffers[] = {vertex_buffer_->GetBuffer()};
        VkDeviceSize   offsets[]  = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, bufffers, offsets);

        if (is_index_available)
        {
            vkCmdBindIndexBuffer(command_buffer, index_buffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void KitMesh::Draw(VkCommandBuffer command_buffer) const
    {
        if (is_index_available)
        {
            vkCmdDrawIndexed(command_buffer, index_count_, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(command_buffer, vertex_count_, 1, 0, 0);
        }
    }

    void KitMesh::CreateVertexBuffers(const std::vector<KitVertex>& vertices)
    {
        vertex_count_ = static_cast<uint32_t>(vertices.size());

        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, vertex_count_ >= 3, "Vertex count must be at least 3!");
        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count_;
        uint32_t     vertex_size = sizeof(vertices[0]);

        // Staging buffer
        KitGraphicsBuffer staging_buffer(
            device_,
            vertex_size,
            vertex_count_,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        staging_buffer.Map();
        staging_buffer.WriteToBuffer(vertices.data());

        // Vertex buffer
        vertex_buffer_ = std::make_unique<KitGraphicsBuffer>(
            device_,
            vertex_size,
            vertex_count_,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Copy staging to vertex
        device_->CopyBuffer(staging_buffer.GetBuffer(), vertex_buffer_->GetBuffer(), buffer_size);
    }

    void KitMesh::CreateIndexBuffers(const std::vector<uint32_t>& indices)
    {
        index_count_ = static_cast<uint32_t>(indices.size());

        is_index_available = index_count_ > 0;
        if (!is_index_available)
        {
            return;
        }

        VkDeviceSize buffer_size = sizeof(indices[0]) * index_count_;
        uint32_t     index_size  = sizeof(indices[0]);

        // Staging buffer
        KitGraphicsBuffer staging_buffer(
            device_,
            index_size,
            index_count_,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        staging_buffer.Map();
        staging_buffer.WriteToBuffer(indices.data());

        // Index buffer
        index_buffer_ = std::make_unique<KitGraphicsBuffer>(
            device_,
            index_size,
            index_count_,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Copy staging to vertex
        device_->CopyBuffer(staging_buffer.GetBuffer(), index_buffer_->GetBuffer(), buffer_size);
    }

    KitModel::KitModel(std::vector<KitMesh>&& meshes) :
        meshes_(std::move(meshes))
    {
    }

    KitModel::~KitModel()
    {
        meshes_.clear();
    }

    void KitModel::AddMesh(KitEngineDevice* device, const KitMeshData& data)
    {
        meshes_.emplace_back(device, data);
    }

    void KitModel::Bind(VkCommandBuffer command_buffer) const
    {
        for (const KitMesh& mesh : meshes_)
        {
            mesh.Bind(command_buffer);
        }
    }

    void KitModel::Draw(VkCommandBuffer command_buffer) const
    {
        for (const KitMesh& mesh : meshes_)
        {
            mesh.Draw(command_buffer);
        }
    }
} // namespace Kitsune
