#include "air_engine.h"

class RotationSc : public Script {
private:
    float t = 0;
public:
    void update(float delta_time) override {
        t += delta_time;
        get_scene().get_component<Transform>(get_entity()).rotation = glm::vec3(t, t, t);
    }
};

class Scene1 : public Scene {
public:
    Entity a;
    Entity cam;

    void on_init() override {
        add_system<RenderingSystem>();
        add_system<ScriptingSystem>();
    }

    void on_start() override {
        cam = create_entity();
        auto& cam_tr = add_component<Transform>(cam);
        add_component<Camera3d>(cam, new Ortho(640, 480), true);

        cam_tr.position = glm::vec3(0, 0, 0);

        a = create_entity();
        auto& sp_sp = add_component<Sprite>(a);
        auto& sp_tr = add_component<Transform>(a);
        add_component<ScriptComponent>(a).bind<RotationSc>();

        sp_sp.size = glm::vec2(100, 100);
        sp_tr.position = glm::vec3(320, 240, -150);
        sp_tr.origin = glm::vec3(50, 50, 0);
    }

    void on_update(float delta_time) override {
        printf("%f \n", 1.0 / delta_time);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}