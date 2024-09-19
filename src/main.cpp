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

class RotationCamera : public Script {
private:
    glm::vec2 m_last_mouse_pos;
public:
    glm::vec2 sensetivity = glm::vec2(0.1);
    float speed = 300;

    void update(float delta_time) override {
        auto& entity_transform = get_scene().get_component<Transform>(get_entity());

        glm::vec2 mouse_pos = Input::get_mouse_position();
        glm::vec2 mouse_delta = m_last_mouse_pos - mouse_pos;
        m_last_mouse_pos = mouse_pos;
        
        // Rotate camera
        entity_transform.rotation += glm::vec3(-mouse_delta.y * sensetivity.x, -mouse_delta.x * sensetivity.y, 0) * delta_time;

        // Calculate forward_dir
        glm::vec3 forward_dir = glm::vec4(0, 0, -1, 1) * glm::eulerAngleXYZ(entity_transform.rotation.x, entity_transform.rotation.y, entity_transform.rotation.z);
        glm::vec3 right_dir = glm::normalize(glm::cross(forward_dir, glm::vec3(0, 1, 0)));

        printf("%f %f %f \n", forward_dir.x, forward_dir.y, forward_dir.z);

        glm::vec3 dir(0);
;       if(Input::is_key_pressed(Key::D))
            dir += right_dir;
        if (Input::is_key_pressed(Key::A))
            dir -= right_dir;
        if (Input::is_key_pressed(Key::W))
            dir += forward_dir;
        if (Input::is_key_pressed(Key::S))
            dir -= forward_dir;

        entity_transform.position += dir * speed * delta_time;
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
        add_component<Camera3d>(cam, new Perspective(640, 480, 3.14f * 45.f/180.f), true);
        add_component<ScriptComponent>(cam).bind<RotationCamera>();

        a = create_entity();
        auto& sp_sp = add_component<Sprite>(a);
        auto& sp_tr = add_component<Transform>(a);
        //add_component<ScriptComponent>(a).bind<RotationSc>();

        sp_sp.size = glm::vec2(100, 100);
        sp_tr.position = glm::vec3(0, 0, -150);
        sp_tr.origin = glm::vec3(50, 50, 0);
    }

    void on_update(float delta_time) override {
        if(Input::is_key_pressed(Key::Space))
            printf("%f \n", 1.0 / delta_time);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}