#include "scenes/scene_rtx_test.h"

int main() {
    Application game(1280, 720, "Fuck my ass");
    game.run(new SceneRtxTest());

    return 0;
}