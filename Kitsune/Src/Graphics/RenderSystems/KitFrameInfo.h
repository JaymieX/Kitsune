#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/KitCamera.h"

namespace Kitsune
{
    struct KitFrameInfo
    {
        int             frame_index;
        float           frame_time;
        VkCommandBuffer command_buffer;
        KitCamera*      camera;
        VkDescriptorSet descriptor_set;
        std::vector<KitGameObject>& game_objects;
    };
} // namespace Kitsune
