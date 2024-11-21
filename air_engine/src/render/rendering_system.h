#pragma once
#include "../core/core.h"
#include "renderer.h"
#include "transform.h"
#include "camera_3d.h"
#include "render_instances.h"
#include "texture_manager.h"

#include <unordered_map>
#include <algorithm>

struct Sprite : public Component {
    glm::vec3 color;
    glm::vec2 size;
    Texture* texture = nullptr;
    glm::vec4 texture_rect = glm::vec4(0, 0, 1, 1);
};

class RenderingSystem : public System {
public:
    TextureManager& get_texture_manager(){
        return m_texture_manager;
    }

private:
    Renderer<vertex> m_renderer;
    TextureManager m_texture_manager;
    std::vector<QuadRenderInstance> m_instances;
    std::unordered_map<GLuint, int> m_textures_count;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(vertex), (const void*)0},
             {0, 2, sizeof(vertex), (const void*)sizeof(glm::vec3)},
             {0, 4, sizeof(vertex), (const void*)(sizeof(glm::vec4) + sizeof(glm::vec3))}
        }, 3 * 100000);

        m_instances.reserve(100000);

        unsigned char default_texture[] = { 255,255,255,255 };
        m_texture_manager.load_texture_by_data(default_texture, { .size_x = 1, .size_y = 1 }, "_default");
    }
    
    void update(float delta_time) override {
        const air_texture_id default_tex_id = m_texture_manager.get_texture("_default")->id;

        auto view_sprites = m_registry->view<Transform, Sprite>();
        view_sprites.each([&](Transform& transform, Sprite& sprite){
            air_texture_id texture_id = sprite.texture ? sprite.texture->id : default_tex_id;

            m_instances.emplace_back(QuadRenderInstance(
                transform.position,
                transform.origin,
                transform.rotation,
                sprite.size,
                texture_id,
                sprite.texture_rect
            ));

            ++m_textures_count[texture_id];
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
        while(it < m_instances.size()){
            glBindTexture(GL_TEXTURE_2D, m_instances[it].texture_id);

            m_renderer.display_part(GL_TRIANGLES, m_textures_count[m_instances[it].texture_id] * QuadRenderInstance::get_count(), it * QuadRenderInstance::get_count());
            it += m_textures_count[m_instances[it].texture_id];

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        m_renderer.clear();
        m_instances.clear();
        m_textures_count.clear();
    }
};