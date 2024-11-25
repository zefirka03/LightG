#pragma once
#include "air_engine.h"

class Scene_test : public Scene {
public:
    Entity ent_cam;
    Entity ent;

    float t = 0;
    float avg_fps = 0;
    int frame_count = 0;

    RenderingSystem* rendering;

    void on_init() override {
        // Systems init
        rendering = add_system<RenderingSystem>();
    }

    void on_start() override {
        ent_cam = create_entity();
        auto& cam = add_component<Camera3d>(ent_cam, new Perspective(640, 360, 3.14f * 60.f / 180.f, 1, 300000), true);
        auto& cam_tr = add_component<Transform>(ent_cam);
        cam_tr.position = glm::vec3(0, 0, -500);

        ent = create_entity();
        auto& sp = add_component<Sprite>(ent);
        auto& sp_tr = add_component<Transform>(ent);
        sp.size = glm::vec2(100);
        sp_tr.origin = glm::vec3(50, 50, 0);
        //sp_tr.position = glm::vec3(20, 20, 0);
    }

    void on_update(float delta_time) override {
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