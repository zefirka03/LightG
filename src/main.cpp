#include "core/air_engine.h"

struct Component1 : public Component {
    int health = 100;
};

class System1 : public System {
public:
    void init() override {

    }

    void update(float delta_time) override {
        printf("System1: delta_time: %f\n", delta_time);
    }
};

class Scene1 : public Scene {
public:
    Entity player;
    void on_start() override {
        add_system<System1>();

        player = create_entity();
        add_component<Component1>(player);
    }

    void on_update(float delta_time) override {
        printf("Scene1: %d\n", get_component<Component1>(player).health);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}