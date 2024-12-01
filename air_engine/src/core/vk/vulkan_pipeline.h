#pragma once
#include <vector>
#include <fstream>
#include "vulkan_device.h"

struct VulkanPipelineConfigInfo{};

class VulkanPipeline{
private:
    VulkanDevice& m_device;
    VkPipeline m_graphics_pipeline;

    std::vector<char> _read_file(std::string const& filepath);

public:
    VulkanPipeline(
        VulkanDevice& device, 
        VulkanPipelineConfigInfo const& config_info
    );
    ~VulkanPipeline();

    VulkanPipeline(VulkanPipeline const&) = delete;
    void operator=(VulkanPipeline const&) = delete;

    static VulkanPipelineConfigInfo default_pipeline_config_info(uint32_t width, uint32_t height);

    void bind(VkCommandBuffer commandBuffer);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);
};