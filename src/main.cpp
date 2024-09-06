#include "core/air_engine.h"
#include "render/render_system.h"

class Scene1 : public Scene {
public:
    Entity a;
    float t = 0;

    void on_init() override {
        add_system<RenderSystem>();
    }

    void on_start() override {
        a = create_entity();
        add_component<Transform>(a);
        add_component<Sprite>(a);
    }

    void on_update(float delta_time) override {
        t += delta_time;
        get_component<Transform>(a).position = glm::vec3(sin(t), cos(t), 0);
        get_component<Transform>(a).size = glm::vec2(0.5, 1);

        printf("%f \n", 1.0 / delta_time);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}