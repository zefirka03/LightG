#pragma once
#include "core/core.h"
#include "render/renderer.h"
#include "transform.h"
#include "camera_3d.h"
#include "render_instances.h"
#include "texture_manager.h"

#include <unordered_map>
#include <algorithm>

struct grass_vertex {
    glm::vec3 position;
};

class GrassSystem : public System {
private:
    Renderer<grass_vertex> m_renderer;

    void init() override {
        m_renderer.reserve({
             {0, 3, sizeof(grass_vertex), (const void*)0},
        } , 3 * 100000);
    }
    
    void update(float delta_time) override {
        
    }
};