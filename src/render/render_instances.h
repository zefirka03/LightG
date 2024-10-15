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

    QuadRenderInstance(
        glm::vec3 position, 
        glm::vec3 origin, 
        glm::vec3 rotation,
        glm::vec2 size,
        GLuint texture_id
    ) : position(position), origin(origin), rotation(rotation), size(size), texture_id(texture_id) {}

    std::array<vertex, 6> get_vertices() const override {
        glm::mat4 rotation_mat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
        glm::vec4 origin4 = glm::vec4(origin, 0);
        glm::vec4 position4 = glm::vec4(position, 0);

        glm::vec3 a_pos = position4 + rotation_mat * (glm::vec4(0, 0, 0, 1)             - origin4);
        glm::vec3 b_pos = position4 + rotation_mat * (glm::vec4(0, size.y, 0, 1)        - origin4);
        glm::vec3 c_pos = position4 + rotation_mat * (glm::vec4(size.x, 0, 0, 1)        - origin4);
        glm::vec3 d_pos = c_pos + b_pos - a_pos;

        return {
            vertex{a_pos, glm::vec2(0, 0)},
            vertex{b_pos, glm::vec2(0, 1)},
            vertex{c_pos, glm::vec2(1, 0)},
            vertex{b_pos, glm::vec2(0, 1)},
            vertex{c_pos, glm::vec2(1, 0)},
            vertex{d_pos, glm::vec2(1, 1)}
        };
    }
};