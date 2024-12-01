#pragma once
#include <vector>
#include <set>
#include <stdexcept>

#include "../ogl.h"
#include "vulkan_window.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanQueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class VulkanDevice {
private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VulkanWindow &m_window;
    VkCommandPool m_command_pool;
    VkDevice m_device;
    VkPhysicalDeviceProperties m_properties;

    VkSurfaceKHR m_surface;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    bool m_enable_validation_layers = true;

    const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    void _create_instance();
    void _setup_debug_messenger();
    void _create_surface();
    void _pick_physical_device();
    void _create_logical_device();
    void _create_command_pool();

    void _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    bool _is_device_suitable(VkPhysicalDevice device);
    bool _check_validation_layer_support();
    bool _check_device_extension_support(VkPhysicalDevice device);
    VulkanQueueFamilyIndices _find_queue_families(VkPhysicalDevice device);
    VulkanQueueFamilyIndices _find_physical_queue_families();
    SwapChainSupportDetails _query_swap_chain_support(VkPhysicalDevice device);
    std::vector<const char*> _get_required_extensions();
    
public:
    VulkanDevice(VulkanWindow &window);
    ~VulkanDevice();

    VulkanDevice(VulkanDevice const&) = delete;
    void operator=(VulkanDevice const&) = delete;
    VulkanDevice(VulkanDevice &&) = delete;
    VulkanDevice &operator=(VulkanDevice &&) = delete;

    VkDevice device() const;
};