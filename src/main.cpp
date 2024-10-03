#include "air_engine.h"

class RotationSc : public Script {
private:
    float t = 0;
    float speed = rand()%100 / 100.f * 5;
    
    Transform* transform;
public:
    void start() override {
        transform = &get_scene().add_component<Transform>(get_entity());
        auto& sp_sp = get_scene().add_component<Sprite>(get_entity());
        auto& sp_pb = get_scene().add_component<PhysicsBody>(get_entity());

        sp_sp.size = glm::vec2((rand() % 100) / 100.f * 200);
        transform->position = glm::vec3((rand() % 1000) / 1000.f * 10000, (rand() % 1000) / 1000.f * 10000, (rand() % 1000) / 1000.f * 10000);
        transform->origin = glm::vec3(sp_sp.size / 2.f, 0);
        sp_pb.set_collider<SpriteCollider>();
        static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
        static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size / 2.f);
        sp_pb.acceleration = glm::vec3(0, 0, 0);
    }
    
    void update(float delta_time) override {
        t += delta_time * speed;
        
        //transform->rotation = glm::vec3(0, t, 0);
        glm::vec3 forward_dir = glm::eulerAngleXYZ(transform->rotation.x, transform->rotation.y, transform->rotation.z) * glm::vec4(0, 0, -1, 1) ;

        get_scene().get_system<DebugSystem>()->draw_line({
            { transform->position, glm::vec4(1,0,0,1) }, 
            { transform->position + forward_dir * 100.f, glm::vec4(0,0,1,1) }
        });
    }
};

class CollisionChecker : public Script {
    float t = 0.1;

    void start() override {
        get_scene().get_component<PhysicsBody>(get_entity()).on_collide_add(this, &CollisionChecker::on_collide);
        t = 3.14 / 2.f;
    }

    void update(float delta_time) override {
        //printf("collisions: %d\n", collisions);
        get_scene().get_component<Transform>(get_entity()).rotation.y = t;
        
    }

    void on_collide(Collider& a, Collider& b) {
        
        printf("%f\n", t);
    }
};

class CameraController : public Script {
private:
    glm::vec2 m_last_mouse_pos;
public:
    glm::vec2 sensetivity = glm::vec2(0.2);
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
        
        //printf("%f %f\n", -mouse_delta.y * sensetivity.x, -mouse_delta.x * sensetivity.y);
        // Rotate camera
        entity_camera.roll += -mouse_delta.y * sensetivity.x * 0.01f;
        entity_camera.yaw += -mouse_delta.x * sensetivity.y * 0.01f;

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

        if (Input::is_key_pressed(Key::LeftShift))
            speed = 800;
        else speed = 300;

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
    Entity front_entity;

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
        add_component<Camera3d>(cam, new Perspective(640, 480, 3.14f * 45.f / 180.f, 0.1, 100000), true);
        add_component<ScriptComponent>(cam).bind<CameraController>();

        {
            front_entity = create_entity();
            auto& sp_sp = add_component<Sprite>(front_entity);
            auto& sp_tr = add_component<Transform>(front_entity);
            auto& sp_pb = add_component<PhysicsBody>(front_entity);
            add_component<ScriptComponent>(front_entity).bind<CollisionChecker>();
            sp_sp.size = glm::vec2(50);
            sp_tr.origin = glm::vec3(sp_sp.size / 2.f, 0);
            sp_pb.set_collider<SpriteCollider>();
            static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size / 2.f);
        }

        for (int i = 0; i < 1500; ++i) {
            a = create_entity();
            add_component<ScriptComponent>(a).bind<RotationSc>();
        }
    }

    void on_update(float delta_time) override {
        get_component<Transform>(front_entity).position = get_component<Transform>(cam).position + get_component<Camera3d>(cam).get_forward() * 500.f;

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