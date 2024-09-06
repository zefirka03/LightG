#pragma once
#include "../core/air_engine.h"
#include "renderer.h"
#include "render_instances.h"

struct Transform : public Component {
    glm::vec3 position;
    glm::vec2 size;
};

struct Sprite : public Component {
    glm::vec3 color;
};

class RenderSystem : public System {
private:
    Renderer m_renderer;
public:
    void init() override {
        m_renderer.reserve(3 * 10000);
    }
    void update(float delta_time) override {
        auto view = m_registry->view<Transform, Sprite>();

        view.each([&](Transform& transform, Sprite& sprite){
            m_renderer.draw(QuadRenderInstance(
                transform.position,
                transform.size
            ));
        });
        m_renderer.display();
    }
};