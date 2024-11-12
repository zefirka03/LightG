#pragma once
#include "core/core.h"
#include "render/renderer.h"
#include "render/transform.h"
#include "render/camera_3d.h"
#include "render/render_instances.h"
#include "render/texture_manager.h"

struct grassData {
    glm::vec3 position;
    float _padding;
    grassData() {}
    grassData(float x, float y, float z) : position(x, y, z) {}
};

class GrassSystem : public System {
private:
    Renderer<glm::vec3> m_renderer;
    objFile m_grass_obj;
    GLuint m_positions_ssbo;
    std::vector<grassData> m_positions_data;
    float m_time = 0;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(glm::vec3), (const void*)0},
        } , 3 * 100);

        m_renderer.get_shader().load_from_file("shaders/grass.shader", AIR_SHADER_VF);

        m_grass_obj.load_from_file("assets/grass.obj");
        m_renderer.draw(m_grass_obj["Plane"].get_v());

        for (int x = 0; x < 400; ++x) {
            for (int z = 0; z < 400; ++z) {
                m_positions_data.emplace_back(((rand() % 10000) / 10000.f) * 25000.f, 0, ((rand() % 10000) / 10000.f) * 25000.f);
            }
        }

        glGenBuffers(1, &m_positions_ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positions_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_positions_data.size() * sizeof(grassData), m_positions_data.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_positions_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    
    void update(float delta_time) override {
        Camera3d* t_main_camera;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if (camera.is_main())
                t_main_camera = &camera;
        });

        m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera");
        m_renderer.get_shader().set_float(m_time, "time");

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positions_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_positions_ssbo);
        m_renderer.display_instances(m_positions_data.size());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_time += delta_time;
    }
};