#pragma once
#include <vector>
#include <fstream>
#include "vulkan_device.h"

struct VulkanPipelineConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class VulkanPipeline{
private:
    VulkanDevice& m_device;
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;

    std::vector<char> _read_file(std::string const& filepath);
    void _create_graphics_pipeline(
      const std::string& vertex_path,
      const std::string& fragment_path,
      const VulkanPipelineConfigInfo& config_info
    );
public:
    VulkanPipeline(
        VulkanDevice& device,
        std::string const& vertex_path,
        std::string const& fragment_path,
        VulkanPipelineConfigInfo const& config_info
    );
    ~VulkanPipeline();

    VulkanPipeline(VulkanPipeline const&) = delete;
    void operator=(VulkanPipeline const&) = delete;

    static VulkanPipelineConfigInfo default_pipeline_config_info(uint32_t width, uint32_t height);

    void bind(VkCommandBuffer commandBuffer);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);
};