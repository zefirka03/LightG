//#include "scenes/scene_test.h"
#include "scenes/scene_vulkan_test.h"

int main() {
    Application game(640, 360, "Fuck my ass");
    game.run(new Scene_vulkan_test());
    
    return 0;
}