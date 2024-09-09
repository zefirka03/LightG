#pragma once
#include "../core/air_engine.h"
#include "../render/renderer.h"
#include "../render/camera_3d.h"
#include "../render/render_instances.h"

struct RTXCanvas : public Component {};

class RenderRTXSystem : public System {
private:
    Renderer m_renderer;
public:
    void init() override {
        m_renderer.reserve(3 * 100);
        m_renderer.get_shader().load_from_file(
            "shaders/rtx.shader",
            AIR_SHADER_VF
        );
    }
    
    void update(float delta_time) override {
        auto view_sprites = m_registry->view<Transform, RTXCanvas>();
        view_sprites.each([&](Transform& transform, RTXCanvas& curve){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.size
            ));
        });

        Camera3d* t_main_camera;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            t_main_camera = &camera;
        });

        m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection(), "camera");
        m_renderer.display();
    }
};