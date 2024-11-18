#pragma once
#include "core/core.h"
#include "core/utils/gpu_vector_storage.h"
#include "render/renderer.h"
#include "render/transform.h"
#include "render/camera_3d.h"
#include "render/render_instances.h"
#include "render/texture_manager.h"
#include "../environment_system/environment_system.h"

struct grassData {
    glm::vec3 position;
    float _padding;
    grassData() {}
    grassData(float x, float y, float z) : position(x, y, z) {}
};

class GrassSystem : public System {
public:
    float world_size = 25000;
    glm::vec2 world_origin = glm::vec2(0);

    void load_map(const char* path) {
        int szx, szy;
        unsigned char* image = SOIL_load_image(path, &szx, &szy, 0, SOIL_LOAD_RGBA);
        if (!image) {
            printf("[AIR][Grass system] file not found\n");
            return;
        }

        m_positions_data.clear();
        for (int x = 0; x < szx; ++x) {
            for (int z = 0; z < szy; ++z) {
                float p_x = x * m_block_size;
                float p_z = z * m_block_size;
                for (int g = 0; g < image[4 * (szx * z + x) + 3] / 5; ++g) {
                    m_positions_data.emplace_back(
                        p_x + ((rand() % 1000) / 1000.f) * m_block_size,
                        0,
                        p_z + ((rand() % 1000) / 1000.f) * m_block_size
                    );
                }
            }
        }
        _gpu_reload_positions();

        SOIL_free_image_data(image);
    }

private:
    Renderer<glm::vec3> m_renderer;
    objFile m_grass_obj;
    GLuint m_positions_ssbo;
    std::vector<grassData> m_positions_data;
    float m_time = 0;
    GPUVectorStorage m_grass_positions;

    float m_block_size = 250;

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
        _gpu_reload_positions();
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
        m_renderer.get_shader().set_float(world_size, "world_size");
        m_renderer.get_shader().set_vector2f(world_origin, "world_origin");

        auto env = m_scene->get_system<EnvironmentSystem>();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_positions_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, env->get_gpu_map_buffer());
        m_renderer.display_instances(m_positions_data.size());

        m_time += delta_time;
    }

    void _gpu_reload_positions() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positions_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_positions_data.size() * sizeof(grassData), m_positions_data.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_positions_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
};