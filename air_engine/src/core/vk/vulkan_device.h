#include "../ogl.h"
#include "../application.h"

class VulkanDevice {
<<<<<<< HEAD
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

public:
=======
>>>>>>> 242911d7ab4d67ecd2369f4133481e24f189cd55
    VulkanDevice(Application &application);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice &) = delete;
    void operator=(const VulkanDevice &) = delete;
    VulkanDevice(VulkanDevice &&) = delete;
    VulkanDevice &operator=(VulkanDevice &&) = delete;
};