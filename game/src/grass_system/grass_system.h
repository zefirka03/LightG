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

    void push_grass(std::vector<grassData> const& data){
        std::vector<grassData> fitted = data;
        for(int i=0; i<fitted.size(); ++i){
            fitted[i].position.x = (fitted[i].position.x + (rand() % 1000) / 1000.f) * m_block_size;
            fitted[i].position.z = (fitted[i].position.z + (rand() % 1000) / 1000.f) * m_block_size;
        }

        m_grass_positions.push_back(fitted.data(), fitted.size() * sizeof(grassData));
    }

    void clear(){
        m_grass_positions.clear();
    }

private:
    Renderer<glm::vec3> m_renderer;
    objFile m_grass_obj;
    GLuint m_positions_ssbo;
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

        m_grass_positions.bind_base(0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, env->get_gpu_map_buffer());
        m_renderer.display_instances(m_grass_positions.size());

        m_time += delta_time;
    }
};