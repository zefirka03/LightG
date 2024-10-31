/*
    TODO:
    This is rtx system, that will render scene with ray tracing tech.

    Usage:
    * RTXCanvas - for entity, that will be textured with framebuffer shader, that will do rtx things
        You need to set camera for RTXCanvas object, that represents projection matrix of camera in RTX space

    * RTXSprite - for entity, that will be rendered in RTX space
*/


#pragma once
#include "../core/core.h"
#include "../render/render.h"

struct RTXCanvas : public Component {
    glm::vec2 size;

    Camera3d* camera = nullptr;
};

struct RTXSprite : public Component {

};

class RenderRTXSystem : public System {
private:
    Renderer<vertex> m_renderer;
    ComputeShader m_compute_shader;
    TextureManager m_texture_manager;
public:
    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(vertex), (const void*)0},
             {0, 2, sizeof(vertex), (const void*)sizeof(glm::vec3)}
        }, 3 * 20);
        m_renderer.get_shader().load_from_file(
            "shaders/rtx_canvas.shader",
            AIR_SHADER_VF
        );
        m_renderer.get_vao().add_ssbo(sizeof(GLfloat) * 3, 3);

        m_compute_shader.load_from_file("shaders/rtx_compute.shader");
        m_texture_manager.texture_storage(TextureStorageParameters({ 640, 360, AIR_TEXTURE_RGBA32F }), "_canvas");
    }
    
    void update(float delta_time) override {
        // Draw and get canvas
        RTXCanvas* t_canvas = nullptr;
        auto view_canvas = m_registry->view<Transform, RTXCanvas>();
        view_canvas.each([&](Transform& transform, RTXCanvas& canvas){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.rotation,
                canvas.size,
                0
            ));
            t_canvas = &canvas;
        });

        // Get main camera
        Camera3d* t_main_camera;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if (camera.is_main())
                t_main_camera = &camera;
        });

        // Send data to shader
        GLfloat c[] = {1, 1, 0.5F };
        m_renderer.get_vao().redata(1, 0, 3 * sizeof(GLfloat), c);
        if(t_canvas && t_canvas->camera)
            m_renderer.get_shader().set_matrix4f(t_canvas->camera->get_projection() * t_canvas->camera->get_view(), "camera");
        //m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera_rtx");

        m_compute_shader.use();
        glBindImageTexture(0, m_texture_manager.get_texture("_canvas")->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glDispatchCompute(640, 360, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Display
        glBindTexture(GL_TEXTURE_2D, m_texture_manager.get_texture("_canvas")->id);
        m_renderer.display();
        glBindTexture(GL_TEXTURE_2D,0);
        m_renderer.clear();
    }
};