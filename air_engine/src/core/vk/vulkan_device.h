#include "../ogl.h"
#include "../application.h"

class VulkanDevice {
    VulkanDevice(Application &application);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice &) = delete;
    void operator=(const VulkanDevice &) = delete;
    VulkanDevice(VulkanDevice &&) = delete;
    VulkanDevice &operator=(VulkanDevice &&) = delete;
};