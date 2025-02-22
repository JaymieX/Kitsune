#pragma once
#include "KitEngineDevice.h"

namespace Kitsune
{
    class KitGraphicsBuffer final
    {
        KitEngineDevice* device_ = nullptr;
        void*            mapped_ = nullptr;
        VkBuffer         buffer_ = VK_NULL_HANDLE;
        VkDeviceMemory   memory_ = VK_NULL_HANDLE;

        VkDeviceSize          buffer_size_           = 0;
        uint32_t              instance_count_        = 0;
        VkDeviceSize          instance_size_         = 0;
        VkDeviceSize          alignment_size_        = 0;
        VkBufferUsageFlags    usage_flags_           = 0;
        VkMemoryPropertyFlags memory_property_flags_ = 0;
        VkDeviceSize          min_offset_alignment_;

    public:
        KitGraphicsBuffer(
            KitEngineDevice*            device,
            const VkDeviceSize          instance_size,
            const uint32_t              instance_count,
            const VkBufferUsageFlags    usage_flags,
            const VkMemoryPropertyFlags memory_property_flags,
            const VkDeviceSize          min_offset_alignment = 1);
        ~KitGraphicsBuffer();

        KitGraphicsBuffer(const KitGraphicsBuffer&)            = delete;
        KitGraphicsBuffer& operator=(const KitGraphicsBuffer&) = delete;

        VkBuffer              GetBuffer() const { return buffer_; }
        void*                 GetMappedMemory() const { return mapped_; }
        uint32_t              GetInstanceCount() const { return instance_count_; }
        VkDeviceSize          GetInstanceSize() const { return instance_size_; }
        VkDeviceSize          GetAlignmentSize() const { return alignment_size_; }
        VkBufferUsageFlags    GetUsageFlags() const { return usage_flags_; }
        VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return memory_property_flags_; }
        VkDeviceSize          GetBufferSize() const { return buffer_size_; }

        VkResult Map(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);
        void     Unmap();

        void                   WriteToBuffer(const void* data, const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) const;
        VkResult               Flush(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) const;
        VkDescriptorBufferInfo DescriptorInfo(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) const;
        VkResult               Invalidate(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) const;

        void                   WriteToIndex(const void* data, const int index) const;
        VkResult               FlushIndex(const int index) const;
        VkDescriptorBufferInfo DescriptorInfoForIndex(const int index) const;
        VkResult               InvalidateIndex(const int index) const;

    private:
        static VkDeviceSize GetAlignment(const VkDeviceSize instance_size, const VkDeviceSize min_offset_alignment);
    };
} // namespace Kitsune
