#pragma once
#include "../ogl.h"

class VulkanWindow{
private:
    GLFWwindow* m_window = nullptr;
    int m_width, m_height;
    const char* m_title;
    
    void _init_window();
public:
    VulkanWindow(int width, int height, const char* title);
    ~VulkanWindow();

    VulkanWindow(const VulkanWindow &) = delete;
    void operator=(const VulkanWindow &) = delete;

    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
    VkExtent2D get_extent() const;
    bool should_close() const;
};