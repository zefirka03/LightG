#pragma once
#include "../core/core.h"
#include "../render/renderer.h"

struct line_vertex {
    glm::vec3 pos;
    glm::vec4 color;
};

struct line {
    line_vertex p1;
    line_vertex p2;
};

class DebugSystem : public System {
public:
    void draw_line(line const& line) {
        m_line_cashed[0] = line.p1;
        m_line_cashed[1] = line.p2;

        m_renderer.draw(m_line_cashed);
    }
private:
    Renderer<line_vertex> m_renderer;
    std::vector<line_vertex> m_line_cashed;

    void init() override {
        m_line_cashed.resize(2);

        m_renderer.reserve({
             {0, 3, sizeof(line_vertex), (const void*)0},
             {0, 4, sizeof(line_vertex), (const void*)sizeof(glm::vec3)},
        }, 3 * 20000);
        m_renderer.get_shader().load_from_file(
            "shaders/line.shader",
            AIR_SHADER_VF
        );
    }
    void start() override {}
    void update(float delta_time) override {
        Camera3d* t_main_camera = nullptr;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if(camera.is_main())
                t_main_camera = &camera;
        });

        if (t_main_camera) {
            m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera");
        }
        m_renderer.display(GL_LINES);
    }
};