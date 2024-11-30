#pragma once
#include "../ogl.h"
#include "../application.h"

class VulkanDevice {
private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    Application &m_window;
    VkCommandPool m_command_pool;
    VkDevice m_device;

    VkSurfaceKHR m_surface;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    void _create_instance();
    void _setup_debug_messenger();
    void _create_surface();
    void _pick_physical_device();
    void _create_logical_device();
    void _create_command_pool();

public:
    VulkanDevice(Application &application);
    ~VulkanDevice();

    VulkanDevice(VulkanDevice const&) = delete;
    void operator=(VulkanDevice const&) = delete;
    VulkanDevice(VulkanDevice &&) = delete;
    VulkanDevice &operator=(VulkanDevice &&) = delete;
};