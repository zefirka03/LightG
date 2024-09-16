#include "core/air_engine.h"
#include "render/render_system.h"

class Scene1 : public Scene {
public:
    Entity a;
    Entity cam;
    float t = 0;

    void on_init() override {
        add_system<RenderSystem>();
    }

    void on_start() override {
        cam = create_entity();
        add_component<Camera3d>(cam, new Ortho(640, 480), true);

        a = create_entity();
        auto& sp_sp = add_component<Sprite>(a);
        auto& sp_tr = add_component<Transform>(a);

        sp_tr.size = glm::vec2(100, 100);
        sp_tr.position = glm::vec3(320, 240, -150);
        sp_tr.origin = glm::vec3(50, 50, 0);
    }

    void on_update(float delta_time) override {
        t += delta_time;

        get_component<Transform>(a).rotation = glm::vec3(t, t, t);

        printf("%f \n", 1.0 / delta_time);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}