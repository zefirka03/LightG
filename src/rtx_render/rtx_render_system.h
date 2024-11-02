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
#include "rtx_core.h"

struct RTX_Canvas : public Component {
    glm::vec2 size;
    Camera3d* camera = nullptr;
};

struct RTX_Object : public Component {
    RTX_Drawable* instance = nullptr;
};

class RenderRTXSystem : public System {
public:
    void draw_debug(DebugSystem& debug_system) {
        auto view_pb = m_registry->view<RTX_Object>();
        view_pb.each([&](RTX_Object& pb) {
            if (pb.instance) {
                auto bbox = pb.instance->get_bounds();
                debug_system.draw_box(bbox.get_a(), bbox.get_b(), glm::vec4(1, 0, 1, 1));
            }
        });
        
        m_quadtree.draw_debug(debug_system, glm::vec4(0.9,0.1,0.1,1));
    }

private:
    Renderer<vertex> m_renderer;
    ComputeShader m_compute_shader;
    TextureManager m_texture_manager;
    Quadtree m_quadtree;
    GLuint m_quadnodes_ssbo;
    GLuint m_childs_ssbo;

    std::vector<RTX_FullChildsPack> m_packed_rtx_objects;

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
        
        glGenBuffers(1, &m_childs_ssbo);
        glGenBuffers(1, &m_quadnodes_ssbo);
    }
    
    void update(float delta_time) override {
        // Draw and get canvas
        RTX_Canvas* t_canvas = nullptr;
        auto view_canvas = m_registry->view<Transform, RTX_Canvas>();
        view_canvas.each([&](Transform& transform, RTX_Canvas& canvas){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.rotation,
                canvas.size,
                0
            ));
            t_canvas = &canvas;
        });

        m_quadtree.clear();
        auto view_rtx_objects = m_registry->view<Transform, RTX_Object>();
        view_rtx_objects.each([&](Transform& transform, RTX_Object& rtx_object){
            rtx_object.instance->m_transform_handler = &transform;
            rtx_object.instance->update_bounds();
            m_quadtree.add_child(rtx_object.instance);
        });
        m_quadtree.devide();

        // Get main camera
        Camera3d* t_main_camera;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if (camera.is_main())
                t_main_camera = &camera;
        });

        // Send data to shader
        if(t_canvas && t_canvas->camera)
            m_renderer.get_shader().set_matrix4f(t_canvas->camera->get_projection() * t_canvas->camera->get_view(), "camera");
        //m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera_rtx");

        // Send quadlist
        _repack_quadlist();
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_childs_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_packed_rtx_objects.size() * sizeof(RTX_FullChildsPack), m_packed_rtx_objects.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_childs_ssbo); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Send nodes
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_quadnodes_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_quadtree.get_quadnodes_pool_size() * sizeof(QuadNode), m_quadtree.get_quadnodes_pool(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_quadnodes_ssbo); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Send camera
        auto& t_main_camera_transform = t_main_camera->scene->get_component<Transform>(t_main_camera->entity);
        glm::vec3 forward_dir = t_main_camera->get_forward();
        glm::vec3 right_dir = glm::normalize(glm::cross(forward_dir, glm::vec3(0, 1, 0)));
        glm::vec3 up_dir = glm::normalize(glm::cross(right_dir, forward_dir));
        m_compute_shader.set_vector3f(t_main_camera_transform.position, std::string("cam.cameraPos").c_str());
        m_compute_shader.set_vector3f(up_dir, std::string("cam.cameraUp").c_str());
        m_compute_shader.set_vector3f(right_dir, std::string("cam.cameraRight").c_str());
        m_compute_shader.set_vector3f(forward_dir, std::string("cam.cameraFront").c_str());
        m_compute_shader.set_float(3.14f * 90.f / 180.f, std::string("cam.fov").c_str());
        m_compute_shader.set_float(0.01f, std::string("cam.dist").c_str());

        // Do rtx compute
        m_compute_shader.use();
        glBindImageTexture(0, m_texture_manager.get_texture("_canvas")->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glDispatchCompute((640 + 7) / 8, (360 + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Display
        glBindTexture(GL_TEXTURE_2D, m_texture_manager.get_texture("_canvas")->id);
        m_renderer.display();
        glBindTexture(GL_TEXTURE_2D,0);
        m_renderer.clear();
    }

    void _repack_quadlist() {
        std::vector<QuadableList> const& quadlist = m_quadtree.get_quadlist();
        m_packed_rtx_objects.clear();
        for (auto& quad : quadlist) {
            auto* drawable = static_cast<RTX_Drawable*>(quad.quad);

            drawable->pack_data();
            m_packed_rtx_objects.emplace_back(
                RTX_ReferencePack(quad.child_next, quad.child_prev, quad.node_it), 
                drawable->m_packed_data
            );
        }
    }
};