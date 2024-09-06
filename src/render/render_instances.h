#pragma once
#include "renderer.h"

struct QuadRenderInstance : public RenderInstance {
    glm::vec3 position;
    glm::vec2 size;

    QuadRenderInstance(glm::vec3 position, glm::vec2 size) : position(position), size(size) {}

    std::vector<vertex> get_vertices() const override {
        glm::vec2 t_size = size / glm::vec2(2.0);
        return {
            {position + glm::vec3(-t_size.x, -t_size.y, 0), glm::vec2(0) },
            {position + glm::vec3(-t_size.x, t_size.y, 0) , glm::vec2(0) },
            {position + glm::vec3(t_size.x, -t_size.y, 0) , glm::vec2(0) },
            {position + glm::vec3(-t_size.x, t_size.y, 0) , glm::vec2(0) },
            {position + glm::vec3(t_size.x, -t_size.y, 0) , glm::vec2(0) },
            {position + glm::vec3(t_size.x, t_size.y, 0)  , glm::vec2(0) }
        };
    }
};