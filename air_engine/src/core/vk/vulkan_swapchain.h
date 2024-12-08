#pragma once
#include <memory>
#include <array>
#include <vector>

#include "../ogl.h"
#include "vulkan_device.h"

class VulkanSwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VulkanSwapChain(VulkanDevice &deviceRef, VkExtent2D windowExtent);
    VulkanSwapChain(
        VulkanDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<VulkanSwapChain> previous);
    ~VulkanSwapChain();

    VulkanSwapChain(const VulkanSwapChain &) = delete;
    VulkanSwapChain &operator=(const VulkanSwapChain &) = delete;

    VkFramebuffer get_frame_buffer(int index) { return swapChainFramebuffers[index]; }
    VkRenderPass get_render_pass() { return renderPass; }
    VkImageView get_image_view(int index) { return swapChainImageViews[index]; }
    size_t image_count() { return swapChainImages.size(); }
    VkFormat get_swap_chain_image_format() { return swapChainImageFormat; }
    VkExtent2D get_swap_chain_extent() { return swapChainExtent; }
    uint32_t width() { return swapChainExtent.width; }
    uint32_t height() { return swapChainExtent.height; }
    float extent_aspect_ratio() {
      return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }
    VkFormat find_depth_format();
    VkResult acquire_next_image(uint32_t *imageIndex);
    VkResult submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
    bool compare_swap_formats(const VulkanSwapChain&swapChain) const {
      return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
             swapChain.swapChainImageFormat == swapChainImageFormat;
    }
private:
    void _init();
    void _create_swap_chain();
    void _create_image_views();
    void _create_depth_resources();
    void _create_render_pass();
    void _create_framebuffers();
    void _create_sync_objects();

    VkSurfaceFormatKHR _choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR _choose_swap_present_mode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D _choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;
    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VulkanDevice &device;
    VkExtent2D windowExtent;
    VkSwapchainKHR swapChain;
    std::shared_ptr<VulkanSwapChain> oldSwapChain;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
};