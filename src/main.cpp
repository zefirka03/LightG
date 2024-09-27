#include "air_engine.h"

class RotationSc : public Script {
private:
    float t = 0;
    float speed = 1;
    
    Transform* transform;
public:
    void start() override {
        transform = &get_scene().get_component<Transform>(get_entity());
    }
    
    void update(float delta_time) override {
        t += delta_time * speed;
        
        transform->rotation = glm::vec3(0, t, 0);
        glm::vec3 forward_dir = glm::eulerAngleXYZ(transform->rotation.x, transform->rotation.y, transform->rotation.z) * glm::vec4(0, 0, -1, 1) ;

        get_scene().get_system<DebugSystem>()->draw_line({
            { transform->position, glm::vec4(1,0,0,1) }, 
            { transform->position + forward_dir * 100.f, glm::vec4(0,0,1,1) }
        });
    }
};

class RotationCamera : public Script {
private:
    glm::vec2 m_last_mouse_pos;
public:
    glm::vec2 sensetivity = glm::vec2(0.1);
    float speed = 300;

    void start() override {
        Input::set_cursor_mode(Cursor::disabled);
    }

    void update(float delta_time) override {
        auto& entity_transform = get_scene().get_component<Transform>(get_entity());
        auto& entity_camera = get_scene().get_component<Camera3d>(get_entity());

        glm::vec2 mouse_pos = Input::get_mouse_position();
        glm::vec2 mouse_delta = mouse_pos - m_last_mouse_pos;
        m_last_mouse_pos = mouse_pos;
        
        // Rotate camera
        entity_camera.roll += -mouse_delta.y * sensetivity.x * 0.01;
        entity_camera.yaw += -mouse_delta.x * sensetivity.y * 0.01;

        // Calculate forward_dir
        glm::vec3 forward_dir = entity_camera.get_forward();
        glm::vec3 right_dir = glm::normalize(glm::cross(forward_dir, glm::vec3(0, 1, 0)));

        // Apply velocity
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
        
        // Hide/Show mouse cursor
        if (Input::is_key_pressed(Key::Escape))
            Input::set_cursor_mode(Cursor::normal);
        if (Input::is_mouse_button_pressed(Mouse::Button0))
            Input::set_cursor_mode(Cursor::disabled);
    }
};

class Scene1 : public Scene {
public:
    Entity a;
    Entity cam;

    DebugSystem* debug;
    PhysicsSystem* physics;

    void on_init() override {
        add_system<RenderingSystem>();
        add_system<ScriptingSystem>();
        debug = add_system<DebugSystem>();
        physics = add_system<PhysicsSystem>();
    }

    void on_start() override {
        cam = create_entity();
        auto& cam_tr = add_component<Transform>(cam);
        add_component<Camera3d>(cam, new Perspective(640, 480, 3.14f * 45.f / 180.f, 0.1, 10000), true);
        add_component<ScriptComponent>(cam).bind<RotationCamera>();

        a = create_entity();
        auto& sp_sp = add_component<Sprite>(a);
        auto& sp_tr = add_component<Transform>(a);
        auto& sp_pb = add_component<PhysicsBody>(a);
        add_component<ScriptComponent>(a).bind<RotationSc>();
        
        sp_sp.size = glm::vec2(100, 100);
        sp_tr.position = glm::vec3(0, 0, 0);
        sp_tr.origin = glm::vec3(50, 50, 0);
        sp_pb.collider = new SpriteCollider();
        dynamic_cast<SpriteCollider*>(sp_pb.collider)->size = glm::vec2(200);
        dynamic_cast<SpriteCollider*>(sp_pb.collider)->origin = glm::vec2(100);
        sp_pb.acceleration = glm::vec3(0, -9, 0);
    }

    void on_update(float delta_time) override {
        // Draw coordinates
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(1,0,0,1)},
            {glm::vec3(100,0,0), glm::vec4(1,0,0,1)}
        });
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(0,1,0,1)},
            {glm::vec3(0,100,0), glm::vec4(0,1,0,1)}
        });
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(0,0,1,1)},
            {glm::vec3(0,0,100), glm::vec4(0,0,1,1)}
        });

        // Draw physics debug
        physics->draw_debug(*debug);

        if(Input::is_key_pressed(Key::Space))
            printf("%f \n", 1.0 / delta_time);
    }
};

int main() {
    Application game(640, 480, "Light");
    game.run(new Scene1());

    return 0;
}