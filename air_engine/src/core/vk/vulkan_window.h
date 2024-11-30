#pragma once
#include "../ogl.h"

class VulkanWindow{
public:
    VulkanWindow(int width, int height, const char* title);
    ~VulkanWindow();

    VulkanWindow(const VulkanWindow &) = delete;
    void operator=(const VulkanWindow &) = delete;
    VulkanWindow(VulkanWindow &&) = delete;
    VulkanWindow &operator=(VulkanWindow &&) = delete;

    void init_window();
    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);

private:
    GLFWwindow* m_window = nullptr;
    int m_width, m_height;
    const char* m_title;
};