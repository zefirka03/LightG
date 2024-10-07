#pragma once
#include "../core/core.h"
#include "renderer.h"
#include "transform.h"
#include "camera_3d.h"
#include "render_instances.h"

struct Sprite : public Component {
    glm::vec3 color;
    glm::vec2 size;
};

class RenderingSystem : public System {
private:
    Renderer<vertex> m_renderer;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(vertex), (const void*)0},
             {0, 2, sizeof(vertex), (const void*)sizeof(glm::vec3)}
        }, 3 * 100000);
    }
    
    void update(float delta_time) override {
        auto view_sprites = m_registry->view<Transform, Sprite>();
        view_sprites.each([&](Transform& transform, Sprite& sprite){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.rotation,
                sprite.size
            ));
        });

        Camera3d* t_main_camera = nullptr;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if(camera.is_main())
                t_main_camera = &camera;
        });

        if (t_main_camera) {
            m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera");
        }
        m_renderer.display();
    }
};