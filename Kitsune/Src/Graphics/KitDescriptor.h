#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "KitEngineDevice.h"

namespace Kitsune
{

    class KitDescriptorSetLayout
    {
    public:
        class KitDescriptorSetLayoutBuilder
        {
        public:
            explicit KitDescriptorSetLayoutBuilder(KitEngineDevice* device) :
                device_(device)
            {
            }

            KitDescriptorSetLayoutBuilder&
            AddBinding(uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, uint32_t count = 1);
            std::unique_ptr<KitDescriptorSetLayout> Build() const;

        private:
            KitEngineDevice* device_;

            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_{};
        };

        KitDescriptorSetLayout(KitEngineDevice* device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~KitDescriptorSetLayout();

        KitDescriptorSetLayout(const KitDescriptorSetLayout&)            = delete;
        KitDescriptorSetLayout& operator=(const KitDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptor_set_layout_; }

    private:
        KitEngineDevice*                                           device_;
        VkDescriptorSetLayout                                      descriptor_set_layout_;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

        friend class KitDescriptorWriter;
    };

    class KitDescriptorPool
    {
    public:
        class KitDescriptorPoolBuilder
        {
        public:
            explicit KitDescriptorPoolBuilder(KitEngineDevice* device) :
                device_(device)
            {
            }

            KitDescriptorPoolBuilder&          AddPoolSize(VkDescriptorType descriptor_type, uint32_t count);
            KitDescriptorPoolBuilder&          SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            KitDescriptorPoolBuilder&          SetMaxSets(uint32_t count);
            std::unique_ptr<KitDescriptorPool> Build() const;

        private:
            KitEngineDevice*                  device_;
            std::vector<VkDescriptorPoolSize> pool_sizes_{};
            uint32_t                          max_sets_   = 1000;
            VkDescriptorPoolCreateFlags       pool_flags_ = 0;
        };

        KitDescriptorPool(
            KitEngineDevice*                         device,
            uint32_t                                 max_sets,
            VkDescriptorPoolCreateFlags              pool_flags,
            const std::vector<VkDescriptorPoolSize>& pool_sizes);
        ~KitDescriptorPool();

        KitDescriptorPool(const KitDescriptorPool&)            = delete;
        KitDescriptorPool& operator=(const KitDescriptorPool&) = delete;

        bool AllocateDescriptor(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const;
        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void ResetPool();

    private:
        KitEngineDevice* device_;
        VkDescriptorPool descriptor_pool_;

        friend class KitDescriptorWriter;
    };

    class KitDescriptorWriter
    {
    public:
        KitDescriptorWriter(KitDescriptorSetLayout& set_layout, KitDescriptorPool& pool);

        KitDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info);
        KitDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* image_info);

        bool Build(VkDescriptorSet& set);
        void Overwrite(VkDescriptorSet& set);

    private:
        KitDescriptorSetLayout&           set_layout_;
        KitDescriptorPool&                pool_;
        std::vector<VkWriteDescriptorSet> writes_;
    };

} // namespace Kitsune
