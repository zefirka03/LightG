#pragma once
#include "vulkan_device.h"

struct VulkanPipelineConfigInfo{};

class VulkanPipeline{
public:
    VulkanPipeline(
        VulkanDevice& device, 
        VulkanPipelineConfigInfo const& config_info
    );

    VulkanPipeline(VulkanPipeline const&) = delete;
    void operator=(VulkanPipeline const&) = delete;

    static VulkanPipelineConfigInfo default_pipeline_config_info(uint32_t width, uint32_t height);

private:
    VulkanDevice& m_device;
};