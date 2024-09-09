#include "core/air_engine.h"
#include "render/render_system.h"
#include "rtx_render/rtx_render_system.h"

class Scene1 : public Scene {
public:
    Entity a;
    Entity cam;
    float t = 0;

    void on_init() override {
        add_system<RenderSystem>();
        add_system<RenderRTXSystem>();
    }

    void on_start() override {
        a = create_entity();
        add_component<Transform>(a);
        add_component<RTXCanvas>(a);

        cam = create_entity();
        add_component<Camera3d>(cam, 640, 480);

        glm::vec2 padding(10, 10);
        get_component<Transform>(a).position = glm::vec3(padding.x, padding.y, 0);
        get_component<Transform>(a).size = glm::vec2(640, 480) - glm::vec2(2) * padding;
    }

    void on_update(float delta_time) override {
        t += delta_time;

        printf("%f \n", 1.0 / delta_time);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}