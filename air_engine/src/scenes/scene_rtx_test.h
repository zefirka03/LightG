#pragma once
#include "air_engine.h"

class RotationSc : public Script {
private:
    float t = 0;
    float destroy_time = 4 + (rand() % 400) / 100.f;
    float speed = 0.5 + rand() % 100 / 100.f * 2;
    glm::vec3 init_velocity;
    
    Transform* transform;
public:
    RotationSc(glm::vec3 _init_velocity){
        init_velocity = _init_velocity;
    }

    void start() override {
        transform = &get_scene().add_component<Transform>(get_entity());
        auto& sp_sp = get_scene().add_component<Sprite>(get_entity());
        sp_sp.texture = get_scene().get_system<RenderingSystem>()->get_texture_manager().get_texture("china");
        sp_sp.size = glm::vec2(100+(rand() % 100) / 100.f * 1000);
        transform->origin = glm::vec3(sp_sp.size / 2.f, 0);
        auto& sp_pb = get_scene().add_component<PhysicsBody>(get_entity());
        sp_pb.type = PhysicsBody::pbType::RIGID;
        sp_pb.bouncyness = (rand()%10)/10.f*1.0f;
        sp_pb.set_collider<SphereCollider>();
            
        auto& rtx_draw = get_scene().add_component<RTX_Object>(get_entity());
        rtx_draw.instance = new RTX_Sphere(sp_sp.size.x / 2);

        sp_pb.friction = 0.99f;
        sp_pb.velocity = init_velocity;
        static_cast<SphereCollider*>(sp_pb.get_collider())->radius = sp_sp.size.x/2;
        sp_pb.tag = 1;
    }
    
    void update(float delta_time) override {
        t += delta_time * speed;
        get_scene().get_component<PhysicsBody>(get_entity()).apply_force(glm::vec3(0, -15.8, 0) * 200.f);
        
        transform->rotation = glm::vec3(0, t, 0);
        glm::vec3 forward_dir = glm::eulerAngleXYZ(transform->rotation.x, transform->rotation.y, transform->rotation.z) * glm::vec4(0, 0, -1, 1);

        if(t/speed > destroy_time)
            get_scene().destroy_entity(get_entity());
    }
};

class RayDraw : public Script{
private:
    float t = 0;
    Ray ray;
    std::vector<std::pair<PhysicsBody*, rayIntersection>> intersections;
    DebugSystem* ds;

public:
    RayDraw(Ray const& _ray, std::vector<std::pair<PhysicsBody*, rayIntersection>>& _intersections){
        ray=_ray;
        intersections = _intersections;
    }

    void start() override {
        ds = get_scene().get_system<DebugSystem>();
    }
    void update(float delta_time) override {
        ds->draw_line(line(ray.origin, ray.origin + ray.length * ray.direction));

        for (int i = 0; i < intersections.size(); ++i) {
            glm::vec3 point = intersections[i].second.points[0].collision_point;
            ds->draw_box(point - glm::vec3(10), point + glm::vec3(10), glm::vec4(1,0,0,1));
        }

        t+=delta_time;
        if(t>5)
            get_scene().destroy_entity(get_entity());
    }
};

class CameraController : public Script {
private:
    glm::vec2 m_last_mouse_pos;

    float m_reload = 0;
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
        ;       if (Input::is_key_pressed(Key::D))
            dir += right_dir;
        if (Input::is_key_pressed(Key::A))
            dir -= right_dir;
        if (Input::is_key_pressed(Key::W))
            dir += forward_dir;
        if (Input::is_key_pressed(Key::S))
            dir -= forward_dir;

        if (Input::is_key_pressed(Key::LeftShift))
            speed = 13500;
        else speed = 1000;

        if (Input::is_mouse_button_pressed(Mouse::Button0) && m_reload > 0.01) {
            std::vector<std::pair<PhysicsBody*, rayIntersection>> out;
            auto ray = Ray(
                entity_transform.position,
                forward_dir,
                55000
            );
            get_scene().get_system<PhysicsSystem>()->ray_intersection(ray, out);

            auto ray_ent = get_scene().create_entity();
            get_scene().add_component<ScriptComponent>(ray_ent).bind<RayDraw>(ray, out);

            if(!out.empty()){
                if(out[0].first->tag == 0){
                    glm::vec3 norm = out[0].second.points[0].normal;
                    for (int i = 0; i < 10; ++i) {
                        auto a = get_scene().create_entity();
                        get_scene().add_component<ScriptComponent>(a).bind<RotationSc>((1000 + (rand()%10000)/10000.f * 15000) * (glm::normalize(ray.direction - 2.f * norm * glm::dot(norm, ray.direction)) + ray.direction * glm::ballRand(0.5f)) );
                        get_scene().get_component<Transform>(a).position = out[0].second.points[0].collision_point + norm * 250.f;
                    }
                }
            }
            m_reload = 0;
            //printf("Ray intersects: %d\n", out.size());
        }

        entity_transform.position += dir * speed * delta_time;
        m_reload += delta_time;
        
        // Hide/Show mouse cursor
        if (Input::is_key_pressed(Key::Escape))
            Input::set_cursor_mode(Cursor::normal);
        if (Input::is_mouse_button_pressed(Mouse::Button0))
            Input::set_cursor_mode(Cursor::disabled);
    }
};

class SceneRtxTest : public Scene {
public:
    Entity a;
    Entity canvas_camera;
    Entity cam;
    Entity plane;
    Entity plane2;
    Entity canvas;
    float t = 0;
    float avg_fps = 0;
    int frame_count = 0;

    DebugSystem* debug;
    PhysicsSystem* physics;
    RenderingSystem* rendering;
    RenderRTXSystem* rtx_rendering;

    void on_init() override {
        physics = add_system<PhysicsSystem>();
        add_system<ScriptingSystem>();
        rendering = add_system<RenderingSystem>();
        rtx_rendering = add_system<RenderRTXSystem>();
        debug = add_system<DebugSystem>();

        physics->set_tags(0, 0, false);
        //physics->set_tags(1, 1, false);
        //physics->set_tags(0, 1, false);

        const GLubyte* version = glGetString(GL_VERSION);
        printf("OGL version supported by this platform: %s\n", version);
    }

    void on_start() override {
        auto& tex_man = rendering->get_texture_manager();
        tex_man.load_texture("img/exp.png", "china");
        tex_man.load_texture("img/tex_checker_1024.png", "stone");

        cam = create_entity();
        auto& cam_tr = add_component<Transform>(cam);
        cam_tr.position = glm::vec3(5000, 5000, 5000);

        int width = Application::get_instance().get_properties().width;
        int height = Application::get_instance().get_properties().height;
        add_component<Camera3d>(cam, new Perspective(width, height, 3.14f * 90.f / 180.f, 0.1, 100000), true);

        add_component<ScriptComponent>(cam).bind<CameraController>();

        {
            plane = create_entity();
            auto& sp_sp = add_component<Sprite>(plane);
            sp_sp.texture = rendering->get_texture_manager().get_texture("stone");
            auto& sp_tr = add_component<Transform>(plane);
            auto& sp_pb = add_component<PhysicsBody>(plane);
            sp_pb.tag = 0;
            sp_sp.size = glm::vec2(200000);
            sp_tr.origin = glm::vec3(sp_sp.size / 2.f, 0);
            sp_tr.rotation.x = glm::half_pi<float>();
            sp_pb.set_collider<PlaneCollider>();

            static_cast<PlaneCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<PlaneCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size / 2.f);

            auto& rtx_draw = add_component<RTX_Object>(plane);
            rtx_draw.instance = new RTX_Plane(sp_tr.position, sp_tr.origin, sp_sp.size, 0);
        }
        {
            plane2 = create_entity();
            auto& sp_sp = add_component<Sprite>(plane2);
            sp_sp.texture = rendering->get_texture_manager().get_texture("stone");
            auto& sp_tr = add_component<Transform>(plane2);
            auto& sp_pb = add_component<PhysicsBody>(plane2);
            sp_sp.size = glm::vec2(12000, 6000);
            sp_tr.origin = glm::vec3(sp_sp.size.x / 2.f, 0, 0);
            sp_tr.position = glm::vec3(6000, 0, 0);
            sp_tr.rotation.y = glm::half_pi<float>();
            sp_pb.tag = 0;
            sp_pb.set_collider<SpriteCollider>();
            //add_component<ScriptComponent>(plane2).bind<CollisionChecker>();

            static_cast<SpriteCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
            static_cast<SpriteCollider*>(sp_pb.get_collider())->origin = glm::vec2(sp_sp.size.x / 2.f, 0);

            auto& rtx_draw = add_component<RTX_Object>(plane2);
            rtx_draw.instance = new RTX_Sprite(sp_tr.position, sp_tr.origin, sp_sp.size, sp_tr.rotation.y);
        }

        {
            canvas_camera = create_entity();
            add_component<Camera3d>(canvas_camera, new Ortho(width, height), false);
            add_component<Transform>(canvas_camera);
            
            canvas = create_entity();
            auto& canvas_canvas = add_component<RTX_Canvas>(canvas);
            auto& canvas_transform = add_component<Transform>(canvas);
            canvas_transform.position = glm::vec3(-640, 0, 1);
            canvas_canvas.camera = &get_component<Camera3d>(canvas_camera);
            canvas_canvas.size = glm::vec2(640, 360);
        }
    }

    void on_update(float delta_time) override {

        // Draw coordinates
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(1,0,0,1)},
            {glm::vec3(1000,0,0), glm::vec4(1,0,0,1)}
        });
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(0,1,0,1)},
            {glm::vec3(0,1000,0), glm::vec4(0,1,0,1)}
        });
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(0,0,1,1)},
            {glm::vec3(0,0,1000), glm::vec4(0,0,1,1)}
        });

        // Draw physics debug
        //physics->draw_debug(*debug);
        rtx_rendering->draw_debug(*debug);

        // Avrg fps
        avg_fps = (avg_fps * frame_count + (1.f / delta_time)) / (frame_count+1);
        ++frame_count;
        t += delta_time;
        if (t > 0.5) {
            printf("Avrg: %f \n", avg_fps);
            t = 0;
        }
        if(frame_count > 50)
            frame_count = 0;
    }
};