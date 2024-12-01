#include "vulkan_window.h"

void VulkanWindow::_init_window(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
}

VulkanWindow::VulkanWindow(int width, int height, const char* title)
 : m_width(width), m_height(height), m_title(title) {
    _init_window();
}

VulkanWindow::~VulkanWindow(){
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void VulkanWindow::create_window_surface(VkInstance instance, VkSurfaceKHR *surface){
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface)) {
        printf("[VulkanWindow] Failed to create window surface!\n");
    }
}

bool VulkanWindow::shouldClose() { return glfwWindowShouldClose(m_window); }