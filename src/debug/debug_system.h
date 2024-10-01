#pragma once
#include "../core/core.h"
#include "../render/renderer.h"

struct line_vertex {
    glm::vec3 pos;
    glm::vec4 color;
};

struct line : RenderInstance<line_vertex, 2> {
    line_vertex p1;
    line_vertex p2;

    line(line_vertex p1, line_vertex p2) : p1(p1), p2(p2) {}

    std::array<line_vertex, 2> get_vertices() const override {
        return {
            p1,
            p2
        };;
    }
};

class DebugSystem : public System {
public:
    void draw_line(line const& line) {
        m_renderer.draw(line);
    }

    void draw_box(glm::vec3 a, glm::vec3 b, glm::vec4 color) {
        draw_line({ {glm::vec3(a.x, a.y, a.z), color}, {glm::vec3(b.x, a.y, a.z), color} });
        draw_line({ {glm::vec3(a.x, a.y, a.z), color}, {glm::vec3(a.x, b.y, a.z), color} });
        draw_line({ {glm::vec3(a.x, a.y, a.z), color}, {glm::vec3(a.x, a.y, b.z), color} });

        draw_line({ {glm::vec3(a.x, b.y, a.z), color}, {glm::vec3(a.x, b.y, b.z), color} });
        draw_line({ {glm::vec3(a.x, b.y, a.z), color}, {glm::vec3(b.x, b.y, a.z), color} });

        draw_line({ {glm::vec3(b.x, a.y, b.z), color}, {glm::vec3(b.x, a.y, a.z), color} });
        draw_line({ {glm::vec3(b.x, a.y, b.z), color}, {glm::vec3(a.x, a.y, b.z), color} });

        draw_line({ {glm::vec3(b.x, a.y, a.z), color}, {glm::vec3(b.x, b.y, a.z), color} });
        draw_line({ {glm::vec3(a.x, b.y, b.z), color}, {glm::vec3(a.x, a.y, b.z), color} });

        draw_line({ {glm::vec3(b.x, b.y, b.z), color}, {glm::vec3(a.x, b.y, b.z), color} });
        draw_line({ {glm::vec3(b.x, b.y, b.z), color}, {glm::vec3(b.x, a.y, b.z), color} });
        draw_line({ {glm::vec3(b.x, b.y, b.z), color}, {glm::vec3(b.x, b.y, a.z), color} });
    }
private:
    Renderer<line_vertex> m_renderer;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(line_vertex), (const void*)0},
             {0, 4, sizeof(line_vertex), (const void*)sizeof(glm::vec3)},
        }, 3 * 80000);
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