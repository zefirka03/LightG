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
    TextureManager& get_texture_manager();
private:
    Renderer<vertex> m_renderer;
    TextureManager m_texture_manager;
    std::vector<QuadRenderInstance> m_instances;
    std::unordered_map<GLuint, int> m_textures_count;

    void init() override;
    void update(float delta_time) override;
};