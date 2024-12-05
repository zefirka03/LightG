//#include "scenes/scene_test.h"
#include "scenes/scene_vulkan_test.h"

class FirstApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    FirstApp() = default;
    ~FirstApp() = default;
    FirstApp(const FirstApp&) = delete;
    FirstApp& operator=(const FirstApp&) = delete;
    void run() {
        while (m_window.should_close()) {
            glfwPollEvents();
        }
    }
private:
    VulkanWindow m_window{ WIDTH, HEIGHT, "Vulkan Tutorial" };
    VulkanDevice m_device{ m_window };
    //VulkanPipeline{}
    // note: order of declarations matters

};

int main() {
    //Application game(640, 360, "Fuck my ass");
    //game.run(new Scene_vulkan_test());

    FirstApp app{};
    
    
    return 0;
}