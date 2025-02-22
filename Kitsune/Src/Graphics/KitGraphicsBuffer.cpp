#include "KitGraphicsBuffer.h"

#include "Core/KitLogs.h"

namespace Kitsune
{
    KitGraphicsBuffer::KitGraphicsBuffer(
        KitEngineDevice*            device,
        const VkDeviceSize          instance_size,
        const uint32_t              instance_count,
        const VkBufferUsageFlags    usage_flags,
        const VkMemoryPropertyFlags memory_property_flags,
        const VkDeviceSize          min_offset_alignment) :
        device_(device),
        instance_count_(instance_count),
        instance_size_(instance_size),
        usage_flags_(usage_flags),
        memory_property_flags_(memory_property_flags),
        min_offset_alignment_(min_offset_alignment)
    {
        alignment_size_ = GetAlignment(instance_size_, min_offset_alignment_);
        buffer_size_    = alignment_size_ * instance_count_;
        device->CreateBuffer(buffer_size_, usage_flags_, memory_property_flags_, buffer_, memory_);
    }

    KitGraphicsBuffer::~KitGraphicsBuffer()
    {
        Unmap();
        vkDestroyBuffer(device_->GetDevice(), buffer_, nullptr);
        vkFreeMemory(device_->GetDevice(), memory_, nullptr);
    }

    VkResult KitGraphicsBuffer::Map(const VkDeviceSize size, const VkDeviceSize offset)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, buffer_ && memory_, "Called map on buffer before create");
        return vkMapMemory(device_->GetDevice(), memory_, offset, size, 0, &mapped_);
    }

    void KitGraphicsBuffer::Unmap()
    {
        if (mapped_)
        {
            vkUnmapMemory(device_->GetDevice(), memory_);
            mapped_ = nullptr;
        }
    }

    void KitGraphicsBuffer::WriteToBuffer(const void* data, const VkDeviceSize size, const VkDeviceSize offset) const
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, mapped_, "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped_, data, buffer_size_);
        }
        else
        {
            char* mem_offset = (char*)mapped_;
            mem_offset += offset;
            memcpy(mem_offset, data, size);
        }
    }

    VkResult KitGraphicsBuffer::Flush(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory              = memory_;
        mapped_range.offset              = offset;
        mapped_range.size                = size;

        return vkFlushMappedMemoryRanges(device_->GetDevice(), 1, &mapped_range);
    }

    VkDescriptorBufferInfo KitGraphicsBuffer::DescriptorInfo(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        return VkDescriptorBufferInfo{
            buffer_,
            offset,
            size,
        };
    }

    VkResult KitGraphicsBuffer::Invalidate(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory              = memory_;
        mapped_range.offset              = offset;
        mapped_range.size                = size;
        return vkInvalidateMappedMemoryRanges(device_->GetDevice(), 1, &mapped_range);
    }

    void KitGraphicsBuffer::WriteToIndex(const void* data, const int index) const
    {
        WriteToBuffer(data, instance_size_, index * alignment_size_);
    }

    VkResult KitGraphicsBuffer::FlushIndex(const int index) const
    {
        return Flush(alignment_size_, index * alignment_size_);
    }

    VkDescriptorBufferInfo KitGraphicsBuffer::DescriptorInfoForIndex(const int index) const
    {
        return DescriptorInfo(alignment_size_, index * alignment_size_);
    }

    VkResult KitGraphicsBuffer::InvalidateIndex(const int index) const
    {
        return Invalidate(alignment_size_, index * alignment_size_);
    }

    VkDeviceSize KitGraphicsBuffer::GetAlignment(const VkDeviceSize instance_size, const VkDeviceSize min_offset_alignment)
    {
        if (min_offset_alignment > 0)
        {
            return (instance_size + min_offset_alignment - 1) & ~(min_offset_alignment - 1);
        }

        return instance_size;
    }
} // namespace Kitsune
