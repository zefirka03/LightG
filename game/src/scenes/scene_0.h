#pragma once
#include "air_engine.h"
#include "../grass_system/grass_system.h"
#include "../environment_system/environment_system.h"
#include "../imgui_system/imgui_system.h"

#include "map_manager/map_manager.h"

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
        sp_sp.texture = get_scene().get_system<RenderingSystem>()->get_texture_manager().get_texture("exp");
        sp_sp.size = glm::vec2(100+(rand() % 100) / 100.f * 1000);
        transform->origin = glm::vec3(sp_sp.size / 2.f, 0);
        auto& sp_pb = get_scene().add_component<PhysicsBody>(get_entity());
        sp_pb.type = PhysicsBody::pbType::RIGID;
        sp_pb.bouncyness = (rand() % 10) / 10.f * 1.0f;
        sp_pb.set_collider<SphereCollider>();
            
        auto& rtx_draw = get_scene().add_component<RTX_Object>(get_entity());
        rtx_draw.instance = new RTX_Sphere(sp_sp.size.x / 2);

        sp_pb.friction = 0.9f;
        sp_pb.velocity = init_velocity;
        static_cast<SphereCollider*>(sp_pb.get_collider())->radius = sp_sp.size.x/2;
        sp_pb.tag = 1;
    }
    
    void update(float delta_time) override {
        t += delta_time * speed;
        get_scene().get_component<PhysicsBody>(get_entity()).apply_force(glm::vec3(0, -15.8, 0) * 200.f);
        transform = &get_scene().get_component<Transform>(get_entity());
        
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
        // Hide/Show mouse cursor
        if (Input::is_key_pressed(Key::Escape))
            Input::set_cursor_mode(Cursor::normal);
        if (Input::is_mouse_button_pressed(Mouse::Button0))
            Input::set_cursor_mode(Cursor::disabled);

        if (Input::get_cursor_mode() == Cursor::normal)
            return;

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
        if (Input::is_key_pressed(Key::D))
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

        if (Input::is_mouse_button_pressed(Mouse::Button0) && m_reload > 0.05) {
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
                        get_scene().add_component<ScriptComponent>(a).bind<RotationSc>(
                            (1000 + (rand()%10000)/10000.f * 15000) * 
                            (glm::normalize(ray.direction - 2.f * norm * glm::dot(norm, ray.direction)) + ray.direction * glm::ballRand(0.5f)) );
                        get_scene().get_component<Transform>(a).position = out[0].second.points[0].collision_point + norm * 250.f;
                    }
                    auto gs = get_scene().get_system<GrassSystem>();
                    auto env = get_scene().get_system<EnvironmentSystem>();
                    int x = int((out[0].second.points[0].collision_point.x - gs->world_origin.x) / (gs->world_size / env->size));
                    int y = int((out[0].second.points[0].collision_point.z - gs->world_origin.y) / (gs->world_size / env->size));

                    printf("%d, %d\n", x, y);
                    
                    float power = 30000;
                    auto mp = env->get_map(x+2, y);
                    int h = 0;

                    if (mp) {
                        mp->v_x = power;
                        mp->v_z = 0;
                    }
                    mp = env->get_map(x - 2, y);
                    if (mp) {
                        mp->v_x = -power;
                        mp->v_z = 0;
                    }
                    mp = env->get_map(x, y+2);
                    if (mp) {
                        mp->v_x = 0;
                        mp->v_z = power;
                    }
                    mp = env->get_map(x, y-2);
                    if (mp) {
                        mp->v_x = 0;
                        mp->v_z = -power;
                    }
                }
            }
            m_reload = 0;
            //printf("Ray intersects: %d\n", out.size());
        }

        entity_transform.position += dir * speed * delta_time;
        m_reload += delta_time;
    }
};

class Scene_0 : public Scene {
public:
    Entity canvas_camera;
    Entity cam;
    Entity plane;
    Entity canvas;
    float t = 0;
    float avg_fps = 0;
    int frame_count = 0;

    float v_key = 0;

    DebugSystem* debug;
    PhysicsSystem* physics;
    RenderingSystem* rendering;
    RenderRTXSystem* rtx_rendering;
    GrassSystem* grass_system;
    ImguiSystem* imgui_system;

    void on_init() override {
        // Systems init
        physics = add_system<PhysicsSystem>();
        add_system<ScriptingSystem>();
        rendering = add_system<RenderingSystem>();
        rtx_rendering = add_system<RenderRTXSystem>();
        debug = add_system<DebugSystem>();
        add_system<EnvironmentSystem>();
        grass_system = add_system<GrassSystem>();
        //imgui_system = add_system<ImguiSystem>();

        // Setup physics
        physics->set_tags(0, 0, false);
        //physics->set_tags(1, 1, false);

        rtx_rendering->set_enabled(false);

        const GLubyte* version = glGetString(GL_VERSION);
        printf("OGL version supported by this platform: %s\n", version);
    }

    void on_start() override {
        // Load textures
        auto& tex_man = rendering->get_texture_manager();
        tex_man.load_texture("assets/img/exp.png", "exp");
        tex_man.load_texture("assets/img/tex_checker_1024.png", "default_1024");
        tex_man.load_texture("assets/img/grass_color.png", "grass_color");

        auto mm_ent = create_entity();
        auto mm = add_component<ScriptComponent>(mm_ent).bind<MapManager>();
        mm->load_map_png("assets/map/map0.map");

        // Create camera
        cam = create_entity();
        auto& cam_tr = add_component<Transform>(cam);
        cam_tr.position = glm::vec3(5000, 5000, 5000);  

        int width = Application::get_instance().get_properties().width;
        int height = Application::get_instance().get_properties().height;
        add_component<Camera3d>(cam, new Perspective(width, height, 3.14f * 60.f / 180.f, 1, 100000), true);

        add_component<ScriptComponent>(cam).bind<CameraController>();

        {
            canvas_camera = create_entity();
            add_component<Camera3d>(canvas_camera, new Ortho(width, height), false);
            add_component<Transform>(canvas_camera);
            
            canvas = create_entity();
            auto& canvas_canvas = add_component<RTX_Canvas>(canvas);
            auto& canvas_transform = add_component<Transform>(canvas);
            canvas_transform.position = glm::vec3(-width, 0, 1);
            canvas_canvas.camera = &get_component<Camera3d>(canvas_camera);
            canvas_canvas.size = glm::vec2(width, height);
        }
    }

    void on_update(float delta_time) override {
        // Draw debug
        //if(imgui_system->physics_draw_debug)
        //    physics->draw_debug(*debug);
        //rtx_rendering->set_enabled(imgui_system->rtx_rendering);

        // Draw coordinates
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(1,0,0,1)},
            {glm::vec3(10000,0,0), glm::vec4(1,0,0,1)}
            });
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(0,1,0,1)},
            {glm::vec3(0,10000,0), glm::vec4(0,1,0,1)}
            });
        debug->draw_line({
            {glm::vec3(0,0,0), glm::vec4(0,0,1,1)},
            {glm::vec3(0,0,10000), glm::vec4(0,0,1,1)}
            });

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