/*
    TODO:
    This is rtx system, that will render scene with ray tracing tech.

    Usage:
    * RTXCanvas - for entity, that will be textured with framebuffer shader, that will do rtx things
        You need to set camera for RTXCanvas object, that represents projection matrix of camera in RTX space

    * RTXSprite - for entity, that will be rendered in RTX space
*/


#pragma once
#include "../core/air_engine.h"
#include "../render/renderer.h"
#include "../render/camera_3d.h"
#include "../render/render_instances.h"

struct RTXCanvas : public Component {
    Camera3d* camera = nullptr;
};

struct RTXSprite : public Component {

};

class RenderRTXSystem : public System {
private:
    Renderer m_renderer;
public:
    void init() override {
        m_renderer.reserve(3 * 20);
        m_renderer.get_shader().load_from_file(
            "shaders/rtx.shader",
            AIR_SHADER_VF
        );
        m_renderer.get_vao().add_ssbo(sizeof(GLfloat) * 3, 3);
    }
    
    void update(float delta_time) override {
        // Draw and get canvas
        RTXCanvas* t_canvas = nullptr;
        auto view_canvas = m_registry->view<Transform, RTXCanvas>();
        view_canvas.each([&](Transform& transform, RTXCanvas& curve){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.size,
                transform.rotation
            ));
            t_canvas = &curve;
        });

        // Get main camera
        Camera3d* t_main_camera;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if (camera.is_main())
                t_main_camera = &camera;
        });

        // Send data to shader
        GLfloat c[] = {1, 1, 1 };
        m_renderer.get_vao().redata(1, 0, 3 * sizeof(GLfloat), c);
        if(t_canvas && t_canvas->camera)
            m_renderer.get_shader().set_matrix4f(t_canvas->camera->get_projection(), "camera_rtx");
        m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection(), "camera");

        // Display
        m_renderer.display();
    }
};