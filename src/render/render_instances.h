#pragma once
#include "renderer.h"

struct QuadRenderInstance : public RenderInstance {
    glm::vec3 position;
    glm::vec3 origin;
    glm::vec2 size;

    QuadRenderInstance(
        glm::vec3 position, 
        glm::vec3 origin, 
        glm::vec2 size
    ) : position(position), origin(origin), size(size) {}

    std::vector<vertex> get_vertices() const override {
        return {
            { position + glm::vec3(0, 0, 0)           - origin, glm::vec2(0, 0) },
            { position + glm::vec3(0, size.y, 0)      - origin, glm::vec2(0, 1) },
            { position + glm::vec3(size.x, 0, 0)      - origin, glm::vec2(1, 0) },
            { position + glm::vec3(0, size.y, 0)      - origin, glm::vec2(0, 1) },
            { position + glm::vec3(size.x, 0, 0)      - origin, glm::vec2(1, 0) },
            { position + glm::vec3(size.x, size.y, 0) - origin, glm::vec2(1, 1) }
        };
    }
};