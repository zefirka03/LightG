#include "scenes/scene_rtx_test.h"

int main() {
    Application game(640, 360, "Light");
    game.run(new SceneRtxTest());

    return 0;
}