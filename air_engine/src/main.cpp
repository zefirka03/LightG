#include "scenes/scene_rtx_test.h"

int main() {
    Application game(1280, 720, "Light");
    game.run(new SceneRtxTest());

    return 0;
}