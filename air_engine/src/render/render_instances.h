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
    ) : position(position), 
        origin(origin), 
        rotation(rotation), 
        size(size), 
        texture_id(texture_id),
        texture_rect(texture_rect) {}

    std::array<vertex, 6> get_vertices() const override {
        glm::mat4 rotation_mat = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
        glm::vec4 origin4 = glm::vec4(origin, 0);
        glm::vec4 position4 = glm::vec4(position, 0);

        glm::vec3 a_pos = position4 + rotation_mat * (glm::vec4(0, 0, 0, 1)             - origin4);
        glm::vec3 b_pos = position4 + rotation_mat * (glm::vec4(0, size.y, 0, 1)        - origin4);
        glm::vec3 c_pos = position4 + rotation_mat * (glm::vec4(size.x, 0, 0, 1)        - origin4);
        glm::vec3 d_pos = c_pos + b_pos - a_pos;

        return {
            vertex{a_pos, glm::vec2(texture_rect.z, texture_rect.w)},
            vertex{b_pos, glm::vec2(texture_rect.z, texture_rect.y)},
            vertex{c_pos, glm::vec2(texture_rect.x, texture_rect.w)},
            vertex{b_pos, glm::vec2(texture_rect.z, texture_rect.y)},
            vertex{c_pos, glm::vec2(texture_rect.x, texture_rect.w)},
            vertex{d_pos, glm::vec2(texture_rect.x, texture_rect.y)}
        };
    }
};