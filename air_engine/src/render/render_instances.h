#pragma once
#include "renderer.h"

struct vertex {
    glm::vec3 position;
    glm::vec2 texture_coordinate;
};

struct QuadRenderInstance : public RenderInstance<vertex, 6> {
    glm::vec3 position;
    glm::vec3 origin;
    glm::vec3 rotation;
    glm::vec2 size;
    GLuint texture_id;
    glm::vec4 texture_rect;

    QuadRenderInstance(
        glm::vec3 position,
        glm::vec3 origin,
        glm::vec3 rotation,
        glm::vec2 size,
        GLuint texture_id,
        glm::vec4 texture_rect = glm::vec4(0, 0, 1, 1)
    );

    std::array<vertex, 6> get_vertices() const override;
};