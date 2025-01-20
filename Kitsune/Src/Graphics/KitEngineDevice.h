#pragma once
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "KitWindow.h"

namespace Kitsune
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        KIT_NODISCARD bool IsComplete() const
        {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   present_modes;

        KIT_NODISCARD bool IsAdequate() const
        {
            return !formats.empty() && !present_modes.empty();
        }
    };
    
    class KitEngineDevice
    {
        KitWindow* window_;
        
#ifdef NDEBUG
        const bool enable_validation_layers_ = false;
#else
        const bool enable_validation_layers_ = true;
#endif

        const std::vector<const char*> validation_layers_ = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> device_extensions_ = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkInstance vk_instance_;

        VkDebugUtilsMessengerEXT debug_messenger_;

        VkSurfaceKHR surface_;

        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
        VkDevice         logical_device_;

        VkQueue graphics_queue_;
        VkQueue present_queue_;

        VkCommandPool command_pool_;

    public:
        explicit KitEngineDevice(KitWindow* window);
        ~KitEngineDevice();

        KitEngineDevice(const KitEngineDevice&) = delete;
        KitEngineDevice(KitEngineDevice&&)      = delete;
        
        KitEngineDevice& operator=(const KitEngineDevice&) = delete;
        KitEngineDevice& operator=(KitEngineDevice&&)      = delete;

        void DeviceWaitIdle() const;
        
        KIT_NODISCARD std::vector<const char*> GetRequiredExtensions() const;

        KIT_NODISCARD bool IsValidationLayerSupported() const;
        KIT_NODISCARD bool IsDeviceSuitable(VkPhysicalDevice device) const;

    private:
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) const;
        
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
        SwapChainSupportDetails QuerySwapChainSupport() const;
    };
}
