#include "KitDescriptor.h"

#include <ranges>

#include "Core/KitLogs.h"

namespace Kitsune
{
#pragma region KitDescriptorSetLayoutBuilder
    KitDescriptorSetLayout::KitDescriptorSetLayoutBuilder& KitDescriptorSetLayout::KitDescriptorSetLayoutBuilder::AddBinding(
        uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, uint32_t count)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, bindings_.count(binding) == 0, "Binding already in use");

        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding         = binding;
        layout_binding.descriptorType  = descriptor_type;
        layout_binding.descriptorCount = count;
        layout_binding.stageFlags      = stage_flags;
        bindings_[binding]             = layout_binding;
        return *this;
    }

    std::unique_ptr<KitDescriptorSetLayout> KitDescriptorSetLayout::KitDescriptorSetLayoutBuilder::Build() const
    {
        return std::make_unique<KitDescriptorSetLayout>(device_, bindings_);
    }
#pragma endregion // KitDescriptorSetLayoutBuilder

#pragma region KitDescriptorSetLayout
    KitDescriptorSetLayout::KitDescriptorSetLayout(
        KitEngineDevice* device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings) :
        device_(device),
        bindings_(bindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
        for (auto val : bindings | std::views::values)
        {
            set_layout_bindings.push_back(val);
        }

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
        descriptor_set_layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
        descriptor_set_layout_info.pBindings    = set_layout_bindings.data();

        const VkResult result =
            vkCreateDescriptorSetLayout(device_->GetDevice(), &descriptor_set_layout_info, nullptr, &descriptor_set_layout_);

        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "failed to create descriptor set layout!");
    }

    KitDescriptorSetLayout::~KitDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(device_->GetDevice(), descriptor_set_layout_, nullptr);
    }
#pragma endregion // KitDescriptorSetLayout

#pragma region KitDescriptorPoolBuilder
    KitDescriptorPool::KitDescriptorPoolBuilder&
    KitDescriptorPool::KitDescriptorPoolBuilder::AddPoolSize(VkDescriptorType descriptor_type, uint32_t count)
    {
        pool_sizes_.emplace_back(descriptor_type, count);
        return *this;
    }

    KitDescriptorPool::KitDescriptorPoolBuilder&
    KitDescriptorPool::KitDescriptorPoolBuilder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
    {
        pool_flags_ = flags;
        return *this;
    }

    KitDescriptorPool::KitDescriptorPoolBuilder& KitDescriptorPool::KitDescriptorPoolBuilder::SetMaxSets(uint32_t count)
    {
        max_sets_ = count;
        return *this;
    }

    std::unique_ptr<KitDescriptorPool> KitDescriptorPool::KitDescriptorPoolBuilder::Build() const
    {
        return std::make_unique<KitDescriptorPool>(device_, max_sets_, pool_flags_, pool_sizes_);
    }
#pragma endregion // KitDescriptorPoolBuilder

#pragma region KitDescriptorPool
    KitDescriptorPool::KitDescriptorPool(
        KitEngineDevice*                         device,
        uint32_t                                 max_sets,
        VkDescriptorPoolCreateFlags              pool_flags,
        const std::vector<VkDescriptorPoolSize>& pool_sizes) :
        device_(device)
    {
        VkDescriptorPoolCreateInfo descriptor_pool_info{};
        descriptor_pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        descriptor_pool_info.pPoolSizes    = pool_sizes.data();
        descriptor_pool_info.maxSets       = max_sets;
        descriptor_pool_info.flags         = pool_flags;

        const VkResult result = vkCreateDescriptorPool(device_->GetDevice(), &descriptor_pool_info, nullptr, &descriptor_pool_);

        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "failed to create descriptor pool!");
    }

    KitDescriptorPool::~KitDescriptorPool()
    {
        vkDestroyDescriptorPool(device_->GetDevice(), descriptor_pool_, nullptr);
    }

    bool KitDescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const
    {
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = descriptor_pool_;
        alloc_info.pSetLayouts        = &descriptor_set_layout;
        alloc_info.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(device_->GetDevice(), &alloc_info, &descriptor) != VK_SUCCESS)
        {
            return false;
        }

        return true;
    }

    void KitDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
    {
        vkFreeDescriptorSets(device_->GetDevice(), descriptor_pool_, static_cast<uint32_t>(descriptors.size()), descriptors.data());
    }

    void KitDescriptorPool::ResetPool()
    {
        vkResetDescriptorPool(device_->GetDevice(), descriptor_pool_, 0);
    }
#pragma endregion // KitDescriptorPool

#pragma region KitDescriptorWriter
    KitDescriptorWriter::KitDescriptorWriter(KitDescriptorSetLayout& set_layout, KitDescriptorPool& pool) :
        set_layout_(set_layout),
        pool_(pool)
    {
    }

    KitDescriptorWriter& KitDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, set_layout_.bindings_.count(binding) == 1, "Layout does not contain specified binding");

        auto& binding_description = set_layout_.bindings_[binding];

        KIT_ASSERT(
            LOG_LOW_LEVEL_GRAPHIC,
            binding_description.descriptorCount == 1,
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType  = binding_description.descriptorType;
        write.dstBinding      = binding;
        write.pBufferInfo     = buffer_info;
        write.descriptorCount = 1;

        writes_.push_back(write);
        return *this;
    }

    KitDescriptorWriter& KitDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* image_info)
    {
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, set_layout_.bindings_.count(binding) == 1, "Layout does not contain specified binding");

        auto& binding_description = set_layout_.bindings_[binding];

        KIT_ASSERT(
            LOG_LOW_LEVEL_GRAPHIC,
            binding_description.descriptorCount == 1,
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType  = binding_description.descriptorType;
        write.dstBinding      = binding;
        write.pImageInfo      = image_info;
        write.descriptorCount = 1;

        writes_.push_back(write);
        return *this;
    }

    bool KitDescriptorWriter::Build(VkDescriptorSet& set)
    {
        const bool success = pool_.AllocateDescriptor(set_layout_.GetDescriptorSetLayout(), set);
        if (!success)
        {
            return false;
        }

        Overwrite(set);
        return true;
    }

    void KitDescriptorWriter::Overwrite(VkDescriptorSet& set)
    {
        for (auto& write : writes_)
        {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(pool_.device_->GetDevice(), writes_.size(), writes_.data(), 0, nullptr);
    }
#pragma endregion // KitDescriptorWriter

} // namespace Kitsune
