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
    glm::vec3 position = glm::vec3(0);
    int chunk_id = -1;
    grassData() {}
    grassData(float x, float y, float z) : position(x, y, z) {}
    grassData(float x, float y, float z, int chunk_id) : position(x, y, z), chunk_id(chunk_id) {}
};

class GrassSystem : public System {
public:
    enum class Resolution {
        high,
        low
    };

    float world_size = 0;
    glm::vec2 world_origin = glm::vec2(0);
    
    void push_grass(std::vector<grassData> const& data, boundingBox bounds, Resolution resolution = Resolution::high) {
        if(resolution == Resolution::high)
            m_gpu_high_grass_positions.push_back(data.data(), data.size() * sizeof(grassData));
        else if(resolution == Resolution::low)
            m_gpu_low_grass_positions.push_back(data.data(), data.size() * sizeof(grassData));
        
        m_bounds.adjust_bounds(bounds);
        glm::vec3 bounds_box = m_bounds.get_b() - m_bounds.get_a();
        world_origin = m_bounds.get_a();
        world_size = std::max(bounds_box.x, bounds_box.z);
    }

    void clear(){
        m_gpu_high_grass_positions.clear();
        m_gpu_low_grass_positions.clear();
        m_bounds = boundingBox();
        world_origin = glm::vec2(FLT_MAX);
        world_size = 0;
    }

private:
    Renderer<glm::vec3> m_renderer;
    objFile m_grass_high_obj;
    objFile m_grass_low_obj;
    GPUVectorStorage m_gpu_high_grass_positions;
    GPUVectorStorage m_gpu_low_grass_positions;
    boundingBox m_bounds;

    float m_time = 0;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(glm::vec3), (const void*)0},
        } , 3 * 100);

        m_renderer.get_shader().load_from_file("shaders/grass.shader", AIR_SHADER_VF);

        m_grass_high_obj.load_from_file("assets/grass.obj");
        m_grass_low_obj.load_from_file("assets/grassTriangle.obj");
    }
    
    void update(float delta_time) override {
        Camera3d* t_main_camera;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if (camera.is_main())
                t_main_camera = &camera;
        });

        auto env = m_scene->get_system<EnvironmentSystem>();

        m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera");
        m_renderer.get_shader().set_float(m_time, "time");
        m_renderer.get_shader().set_int(env->size, "size");
        m_renderer.get_shader().set_float(world_size, "world_size");
        m_renderer.get_shader().set_vector2f(world_origin, "world_origin");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, env->get_gpu_map_buffer());

        m_renderer.draw(m_grass_high_obj["Plane"].get_v());
        m_gpu_high_grass_positions.bind_base(0);
        m_renderer.display_instances(m_gpu_high_grass_positions.size() / sizeof(grassData));
        m_renderer.clear();

        m_renderer.draw(m_grass_low_obj["Plane"].get_v());
        m_gpu_low_grass_positions.bind_base(0);
        m_renderer.display_instances(m_gpu_low_grass_positions.size() / sizeof(grassData));
        m_renderer.clear();

        m_time += delta_time;
    }
};