//#include "scenes/scene_test.h"
#include "scenes/scene_vulkan_test.h"

class FirstApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    FirstApp() {
        create_pipeline_layout();
        create_pipeline();
        create_command_buffers();
    }
    ~FirstApp() = default;
    FirstApp(const FirstApp&) = delete;
    FirstApp& operator=(const FirstApp&) = delete;

    void draw_frame() {
        uint32_t imageIndex;
        auto result = m_swapchain.acquire_next_image(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        result = m_swapchain.submit_command_buffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void create_pipeline_layout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void create_pipeline() {
        VulkanPipelineConfigInfo pipelineConfig{};
        VulkanPipeline::default_pipeline_config_info(
            pipelineConfig,
            m_swapchain.width(),
            m_swapchain.height());
        pipelineConfig.renderPass = m_swapchain.get_render_pass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        m_pipeline = std::make_unique<VulkanPipeline>(
            m_device,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void create_command_buffers() {
        commandBuffers.resize(m_swapchain.image_count());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_device.get_command_pool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(m_device.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_swapchain.get_render_pass();
            renderPassInfo.framebuffer = m_swapchain.get_frame_buffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = m_swapchain.get_swap_chain_extent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_pipeline->bind(commandBuffers[i]);
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void run() {
        while (!m_window.should_close()) {
            glfwPollEvents();
            draw_frame();
        }
    }
private:
    VulkanWindow m_window{ WIDTH, HEIGHT, "Vulkan Tutorial" };
    VulkanDevice m_device{ m_window };
    VulkanSwapChain m_swapchain{m_device, m_window.get_extent()};
    std::unique_ptr<VulkanPipeline> m_pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};

int main() {
    //Application game(640, 360, "Fuck my ass");
    //game.run(new Scene_vulkan_test());

    FirstApp app{};
    app.run();
    
    return 0;
}