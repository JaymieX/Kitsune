﻿#include "KitEngineDevice.h"

#include <set>

#include "Core/KitLogs.h"

#include <unordered_set>

// Helper functions
namespace
{
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
            VkDebugUtilsMessageTypeFlagsEXT             message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
            void*                                       p_user_data)
    {
        switch (message_severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_INFO, p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_INFO, p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_WARNING, p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_ERROR, p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_INFO, p_callback_data->pMessage);
            break;
        }

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance                                instance,
        const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
        const VkAllocationCallbacks*              p_allocator,
        VkDebugUtilsMessengerEXT*                 p_debug_messenger)
    {
        if (const auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"); func != nullptr)
        {
            return func(instance, p_create_info, p_allocator, p_debug_messenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(
        VkInstance                   instance,
        VkDebugUtilsMessengerEXT     debug_messenger,
        const VkAllocationCallbacks* p_allocator)
    {
        if (const auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"); func != nullptr)
        {
            func(instance, debug_messenger, p_allocator);
        }
    }
}

namespace Kitsune
{
    KitEngineDevice::KitEngineDevice(KitWindow* window):
        window_(window)
    {
        KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_INFO, "Creating engine device");
        
        if (enable_validation_layers_)
        {
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, IsValidationLayerSupported(), "Validation layers are enabled but not supported!");
        }

        // --- Create instance ---
        {
            VkApplicationInfo app_info{};
            app_info.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            app_info.pApplicationName    = "Kitsune Tools";
            app_info.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
            app_info.pEngineName         = "No Engine";
            app_info.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
            app_info.apiVersion          = VK_API_VERSION_1_0;

            const std::vector<const char*> required_extensions = GetRequiredExtensions();

            VkInstanceCreateInfo create_info{};
            create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.pApplicationInfo        = &app_info;
            create_info.enabledExtensionCount   = static_cast<uint32_t>(required_extensions.size());
            create_info.ppEnabledExtensionNames = required_extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
            if (enable_validation_layers_)
            {
                create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
                create_info.ppEnabledLayerNames = validation_layers_.data();

                // Instance happens first so we need to direct its debug message here
                PopulateDebugMessengerCreateInfo(debug_create_info);
                create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
            }
            else
            {
                create_info.enabledLayerCount = 0;
                create_info.pNext = nullptr;
            }

            VkResult result = vkCreateInstance(&create_info, nullptr, &vk_instance_);
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create instance!");
        }
        // --- End create instance ---

        // --- Setup validation layers ---
        if (enable_validation_layers_)
        {
            VkDebugUtilsMessengerCreateInfoEXT create_info;
            PopulateDebugMessengerCreateInfo(create_info);

            VkResult result = CreateDebugUtilsMessengerEXT(vk_instance_, &create_info, nullptr, &debug_messenger_);
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Layer not failed to set up debug messenger!");
        }
        // --- End setup validation layers ---

        // --- Create surface ---
        {
            VkResult result = glfwCreateWindowSurface(vk_instance_, window_->window_, nullptr, &surface_);
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create window surface!");
        }
        // --- End create surface ---

        // --- Picking physical device ---
        {
            uint32_t device_count = 0;
            vkEnumeratePhysicalDevices(vk_instance_, &device_count, nullptr);

            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, device_count != 0, "failed to find GPUs with Vulkan support!");

            std::vector<VkPhysicalDevice> devices(device_count);
            vkEnumeratePhysicalDevices(vk_instance_, &device_count, devices.data());

            for (const auto& device : devices)
            {
                if (IsDeviceSuitable(device))
                {
                    physical_device_ = device;
                    break;
                }
            }

            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, physical_device_ != VK_NULL_HANDLE, "failed to find a suitable GPU!");

            vkGetPhysicalDeviceProperties(physical_device_, &properties);
        }
        // --- End picking physical device ---

        // --- Creating logical device ---
        {
            QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

            std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
            std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };
            
            float queue_priority = 1.0f;

            for (uint32_t queue_family : unique_queue_families)
            {
                VkDeviceQueueCreateInfo queue_create_info{};
                queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_create_info.queueFamilyIndex = queue_family;
                queue_create_info.queueCount       = 1;
                queue_create_info.pQueuePriorities = &queue_priority;

                queue_create_infos.push_back(queue_create_info);
            }

            VkPhysicalDeviceFeatures device_features{};

            VkDeviceCreateInfo create_info{};
            create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.pQueueCreateInfos       = queue_create_infos.data();
            create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
            create_info.pEnabledFeatures        = &device_features;
            create_info.enabledExtensionCount   = static_cast<uint32_t>(device_extensions_.size());
            create_info.ppEnabledExtensionNames = device_extensions_.data();

            if (enable_validation_layers_)
            {
                create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers_.size());
                create_info.ppEnabledLayerNames = validation_layers_.data();
            }
            else
            {
                create_info.enabledLayerCount = 0;
            }

            VkResult result = vkCreateDevice(physical_device_, &create_info, nullptr, &logical_device_);

            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create link to physical device!");

            vkGetDeviceQueue(logical_device_, indices.graphics_family.value(), 0, &graphics_queue_);
            vkGetDeviceQueue(logical_device_, indices.present_family.value(), 0, &present_queue_);
        }
        // --- End creating logical device ---

        // --- Create command pool ---
        {
            QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);

            VkCommandPoolCreateInfo pool_info = {};
            pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
            pool_info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            VkResult result = vkCreateCommandPool(logical_device_, &pool_info, nullptr, &command_pool_);
        
            KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create command pool!");
        }
        // --- End create command pool ---
    }

    KitEngineDevice::~KitEngineDevice()
    {
        KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, Kitsune::KitLogLevel::LOG_INFO, "Destroying engine device");
        vkDestroyCommandPool(logical_device_, command_pool_, nullptr);
        
        if (enable_validation_layers_)
        {
            DestroyDebugUtilsMessengerEXT(vk_instance_, debug_messenger_, nullptr);
        }
        
        vkDestroyDevice(logical_device_, nullptr);
        vkDestroySurfaceKHR(vk_instance_, surface_, nullptr);
        vkDestroyInstance(vk_instance_, nullptr);
    }

    void KitEngineDevice::DeviceWaitIdle() const
    {
        vkDeviceWaitIdle(logical_device_);
    }

    std::vector<const char*> KitEngineDevice::GetRequiredExtensions() const
    {
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

        if (enable_validation_layers_)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool KitEngineDevice::IsValidationLayerSupported() const
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

        // Gather all the validation layers available to our machine
        std::unordered_set<std::string> available_layers;
        for (const auto& layer_properties : layers)
        {
            available_layers.insert(layer_properties.layerName);
        }

        // Check that the validation layers we want to use is indeed available
        for (auto layer_name : validation_layers_)
        {
            if (!available_layers.contains(layer_name))
            {
                KIT_LOG(LOG_LOW_LEVEL_GRAPHIC, KitLogLevel::LOG_ERROR, "Layer not supported: {}", layer_name);
                return false;
            }
        }

        return true;
    }

    bool KitEngineDevice::IsDeviceSuitable(VkPhysicalDevice device) const
    {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);

        QueueFamilyIndices family_indices = FindQueueFamilies(device);

        const bool is_extension_supported = CheckDeviceExtensionSupport(device);
        bool swap_chain_adequate = false;

        if (is_extension_supported)
        {
            SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
            swap_chain_adequate = swap_chain_support.IsAdequate();
        }

        return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            device_features.geometryShader &&
            family_indices.IsComplete() && is_extension_supported && swap_chain_adequate;
    }

    void KitEngineDevice::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) const
    {
        create_info = {};

        create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = DebugCallback;
        create_info.pUserData       = nullptr;
    }

    void KitEngineDevice::CreateImageWithInfo(
        const VkImageCreateInfo& image_info,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& image_memory) const
    {
        VkResult result = vkCreateImage(logical_device_, &image_info, nullptr, &image);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to create image view!");

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(logical_device_, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize  = mem_requirements.size;
        alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

        result = vkAllocateMemory(logical_device_, &alloc_info, nullptr, &image_memory);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to allocate image memory!");

        result = vkBindImageMemory(logical_device_, image, image_memory, 0);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Fail to bind image memory!");
    }

    void KitEngineDevice::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory) const
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size        = size;
        buffer_info.usage       = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(logical_device_, &buffer_info, nullptr, &buffer);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Failed to create vertex buffer!");

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(logical_device_, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize  = mem_requirements.size;
        alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

        result = vkAllocateMemory(logical_device_, &alloc_info, nullptr, &buffer_memory);
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, result == VK_SUCCESS, "Failed to allocate vertex buffer memory!");

        vkBindBufferMemory(logical_device_, buffer, buffer_memory, 0);
    }

    VkCommandBuffer KitEngineDevice::BeginSingleTimeCommands() const
    {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool        = command_pool_;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(logical_device_, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        return command_buffer;
    }

    void KitEngineDevice::EndSingleTimeCommands(VkCommandBuffer command_buffer) const
    {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = &command_buffer;

        vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue_);

        vkFreeCommandBuffers(logical_device_, command_pool_, 1, &command_buffer);
    }

    void KitEngineDevice::CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) const
    {
        VkCommandBuffer command_buffer = BeginSingleTimeCommands();

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;  // Optional
        copy_region.dstOffset = 0;  // Optional
        copy_region.size      = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        EndSingleTimeCommands(command_buffer);
    }

    void KitEngineDevice::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count)
    {
        VkCommandBuffer command_buffer = BeginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset      = 0;
        region.bufferRowLength   = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = layer_count;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(
            command_buffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
        
        EndSingleTimeCommands(command_buffer);
    }

    QueueFamilyIndices KitEngineDevice::FindQueueFamilies(VkPhysicalDevice device) const
    {
        QueueFamilyIndices indices;
        // Logic to find queue family indices to populate struct with

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        int i = 0;
        for (const auto& queue_family : queue_families)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics_family = i;
            }

            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);

            if (present_support)
            {
                indices.present_family = i;
            }

            if (indices.IsComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    QueueFamilyIndices KitEngineDevice::FindQueueFamilies() const
    {
        return FindQueueFamilies(physical_device_);
    }

    VkFormat KitEngineDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, false, "Fail to locate appropriate format!");
        return VK_FORMAT_MAX_ENUM;
    }

    bool KitEngineDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());

        for (const auto& extension : available_extensions)
        {
            required_extensions.erase(extension.extensionName);
        }

        return required_extensions.empty();
    }

    SwapChainSupportDetails KitEngineDevice::QuerySwapChainSupport(VkPhysicalDevice device) const
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);

        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);

        if (present_mode_count != 0)
        {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes.data());
        }

        return details;
    }

    SwapChainSupportDetails KitEngineDevice::QuerySwapChainSupport() const
    {
        return QuerySwapChainSupport(physical_device_);
    }

    uint32_t KitEngineDevice::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        
        KIT_ASSERT(LOG_LOW_LEVEL_GRAPHIC, false, "Fail to locate appropriate memory type!");
        return 0;
    }
}
