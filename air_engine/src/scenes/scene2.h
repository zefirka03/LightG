#pragma once
#include "air_engine.h"

class RotationSc : public Script {
private:
    float t = 0;
    float speed = rand() % 100 / 100.f * 5;
    
    Transform* transform;
public:
    void start() override {
        transform = &get_scene().add_component<Transform>(get_entity());
        auto& sp_sp = get_scene().add_component<Sprite>(get_entity());
        sp_sp.size = glm::vec2(100+(rand() % 100) / 100.f * 100);
        transform->position = glm::vec3(400 - (rand() % 800), 400 + (rand() % 800), 400 - (rand() % 800));
        transform->origin = glm::vec3(sp_sp.size / 2.f, 0);
        auto& sp_pb = get_scene().add_component<PhysicsBody>(get_entity());
        sp_pb.type = PhysicsBody::pbType::RIGID;
        sp_pb.bouncyness = (rand()%100)/100.f*0.7f;
        sp_pb.set_collider<SphereCollider>();
        sp_pb.velocity = glm::vec3(0);
        static_cast<SphereCollider*>(sp_pb.get_collider())->radius = sp_sp.size.x/2;
        sp_pb.tag = 1;
    }
    
    void update(float delta_time) override {
        t += delta_time * speed;
        get_scene().get_component<PhysicsBody>(get_entity()).apply_force(glm::vec3(0, -9.8, 0) * 200.f);
        
        transform->rotation = glm::vec3(0, t, 0);
        glm::vec3 forward_dir = glm::eulerAngleXYZ(transform->rotation.x, transform->rotation.y, transform->rotation.z) * glm::vec4(0, 0, -1, 1);
    }
};


class CameraController : public Script {
private:
    glm::vec2 m_last_mouse_pos;
public:
    glm::vec2 sensetivity = glm::vec2(0.2);
    float speed = 600;

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
            speed = 3500;
        else speed = 1000;

        entity_transform.position += dir * speed * delta_time;
        
        // Hide/Show mouse cursor
        if (Input::is_key_pressed(Key::Escape))
            Input::set_cursor_mode(Cursor::normal);
        if (Input::is_mouse_button_pressed(Mouse::Button0))
            Input::set_cursor_mode(Cursor::disabled);
    }
};

class Scene2 : public Scene {
public:
    Entity a;
    Entity cam;
    Entity plane;
    Entity plane2;
    Entity plane3;
    Entity plane4;
    Entity plane5;
    float t = 0;

    DebugSystem* debug;
    PhysicsSystem* physics;

    void on_init() override {
        add_system<RenderingSystem>();
        add_system<ScriptingSystem>();
        debug = add_system<DebugSystem>();
        physics = add_system<PhysicsSystem>();

        physics->set_tags(0, 0, false);
        //physics->set_tags(1, 1, false);
    }

    void on_start() override {
        cam = create_entity();
        auto& cam_tr = add_component<Transform>(cam);
        cam_tr.position = glm::vec3(5000, 5000, 5000);

        int width = Application::get_instance().get_properties().width;
        int height = Application::get_instance().get_properties().height;
        add_component<Camera3d>(cam, new Perspective(width, height, 3.14f * 45.f / 180.f, 0.1, 100000), true);

        add_component<ScriptComponent>(cam).bind<CameraController>();

        {
            plane = create_entity();
            auto& sp_sp = add_component<Sprite>(plane);
            auto& sp_tr = add_component<Transform>(plane);
            auto& sp_pb = add_component<PhysicsBody>(plane);
            sp_pb.tag = 0;
            sp_sp.size = glm::vec2(2000);
            sp_tr.origin = glm::vec3(sp_sp.size / 2.f, 0);
            sp_tr.rotation.x = glm::half_pi<float>();
            sp_pb.set_collider<PlaneCollider>();
            //add_component<ScriptComponent>(plane).bind<CollisionChecker>();
            static_cast<PlaneCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<PlaneCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size / 2.f);
        }
        {
            plane2 = create_entity();
            auto& sp_sp = add_component<Sprite>(plane2);
            auto& sp_tr = add_component<Transform>(plane2);
            auto& sp_pb = add_component<PhysicsBody>(plane2);
            sp_sp.size = glm::vec2(2000, 2000);
            sp_tr.origin = glm::vec3(sp_sp.size.x / 2.f, 0, 0);
            sp_tr.position = glm::vec3(1000, 0, 0);
            sp_tr.rotation.y = glm::half_pi<float>();
            sp_pb.tag = 0;
            sp_pb.set_collider<SpriteCollider>();
            //add_component<ScriptComponent>(plane3).bind<CollisionChecker>();

            static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size.x / 2.f, 0);
        }
        {
            plane3 = create_entity();
            auto& sp_sp = add_component<Sprite>(plane3);
            auto& sp_tr = add_component<Transform>(plane3);
            auto& sp_pb = add_component<PhysicsBody>(plane3);
            sp_sp.size = glm::vec2(2000, 2000);
            sp_tr.origin = glm::vec3(sp_sp.size.x / 2.f, 0, 0);
            sp_tr.position = glm::vec3(-1000, 0, 0);
            sp_tr.rotation.y = glm::half_pi<float>();
            sp_pb.tag = 0;
            sp_pb.set_collider<SpriteCollider>();
            //add_component<ScriptComponent>(plane3).bind<CollisionChecker>();

            static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size.x / 2.f, 0);
        }
        {
            plane4 = create_entity();
            auto& sp_sp = add_component<Sprite>(plane4);
            auto& sp_tr = add_component<Transform>(plane4);
            auto& sp_pb = add_component<PhysicsBody>(plane4);
            sp_sp.size = glm::vec2(2000, 2000);
            sp_tr.origin = glm::vec3(sp_sp.size.x / 2.f, 0, 0);
            sp_tr.position = glm::vec3(0, 0, -1000);
            sp_pb.tag = 0;
            sp_pb.set_collider<SpriteCollider>();
            //add_component<ScriptComponent>(plane3).bind<CollisionChecker>();

            static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size.x / 2.f, 0);
        }
        {
            plane5 = create_entity();
            auto& sp_sp = add_component<Sprite>(plane5);
            auto& sp_tr = add_component<Transform>(plane5);
            auto& sp_pb = add_component<PhysicsBody>(plane5);
            sp_sp.size = glm::vec2(2000, 2000);
            sp_tr.origin = glm::vec3(sp_sp.size.x / 2.f, 0, 0);
            sp_tr.position = glm::vec3(0, 0, 1000);
            sp_pb.tag = 0;
            sp_pb.set_collider<SpriteCollider>();
            //add_component<ScriptComponent>(plane3).bind<CollisionChecker>();

            static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size.x / 2.f, 0);
        }   
        
        for (int i = 0; i < 500; ++i) {
            a = create_entity();
            add_component<ScriptComponent>(a).bind<RotationSc>();
        }
    }

    void on_update(float delta_time) override {
        //get_component<Transform>(front_entity).position = get_component<Transform>(cam).position + get_component<Camera3d>(cam).get_forward() * 500.f;

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
        //physics->draw_debug(*debug);

        if(Input::is_key_pressed(Key::Space))
            printf("%f \n", 1.0 / delta_time);
    }
};