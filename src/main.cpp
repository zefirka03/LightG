#include "scenes/scene1.h"

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}