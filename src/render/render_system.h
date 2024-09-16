#pragma once
#include "../core/air_engine.h"
#include "renderer.h"
#include "camera_3d.h"
#include "render_instances.h"

struct Transform : public Component {
    glm::vec3 position;
    glm::vec3 origin;
    glm::vec2 size;

    glm::vec3 rotation;
};

struct Sprite : public Component {
    glm::vec3 color;
};

class RenderSystem : public System {
private:
    Renderer m_renderer;
public:
    void init() override {
        m_renderer.reserve(3 * 10000);
    }
    
    void update(float delta_time) override {
        auto view_sprites = m_registry->view<Transform, Sprite>();
        view_sprites.each([&](Transform& transform, Sprite& sprite){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.size,
                transform.rotation
            ));
        });

        Camera3d* t_main_camera = nullptr;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if(camera.is_main())
                t_main_camera = &camera;
        });

        m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection(), "camera");
        m_renderer.display();
    }
};