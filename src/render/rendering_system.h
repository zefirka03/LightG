#pragma once
#include "../core/core.h"
#include "renderer.h"
#include "transform.h"
#include "camera_3d.h"
#include "render_instances.h"

#include <unordered_map>
#include <algorithm>

struct Sprite : public Component {
    glm::vec3 color;
    glm::vec2 size;
    GLuint texture_id;
};

class RenderingSystem : public System {
private:
    Renderer<vertex> m_renderer;
    std::vector<QuadRenderInstance> m_instances;
    std::unordered_map<GLuint, int> m_textures_count;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(vertex), (const void*)0},
             {0, 2, sizeof(vertex), (const void*)sizeof(glm::vec3)}
        }, 3 * 100000);

        m_instances.reserve(100000);
    }
    
    void update(float delta_time) override {
        auto view_sprites = m_registry->view<Transform, Sprite>();
        view_sprites.each([&](Transform& transform, Sprite& sprite){
            m_instances.emplace_back(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.rotation,
                sprite.size,
                sprite.texture_id
            ));

            ++m_textures_count[sprite.texture_id];
        });

        std::sort(m_instances.begin(), m_instances.end(), [](QuadRenderInstance const& a, QuadRenderInstance const& b){
            return a.texture_id < b.texture_id;
        });

        for(int i = 0; i < m_instances.size(); ++i)
            m_renderer.draw(m_instances[i]);
        m_renderer.load_all();

        Camera3d* t_main_camera = nullptr;
        auto view_cameras = m_registry->view<Camera3d>();
        view_cameras.each([&](Camera3d& camera){
            if(camera.is_main())
                t_main_camera = &camera;
        });

        if (t_main_camera) 
            m_renderer.get_shader().set_matrix4f(t_main_camera->get_projection() * t_main_camera->get_view(), "camera");

        //m_renderer.display();

        int it = 0;
        while(it < m_instances.size() * QuadRenderInstance::get_count()){
            m_renderer.display_part(GL_TRIANGLES, m_textures_count[m_instances[it].texture_id] * QuadRenderInstance::get_count(), it);
            it += m_textures_count[m_instances[it].texture_id] * QuadRenderInstance::get_count();
        }

        m_renderer.clear();
        m_instances.clear();
        m_textures_count.clear();
    }
};