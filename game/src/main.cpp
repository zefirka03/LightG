#include "scenes/scene_0.h"

int main() {
    Application game(1920, 1080, "test", true);
    //Application game(640, 360, "test");
    game.run(new Scene_0());

    return 0;
}