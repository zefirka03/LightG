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

        return {
            vertex{position4 + rotation_mat * (glm::vec4(0, 0, 0, 1)            - origin4), glm::vec2(0, 0)},
            vertex{position4 + rotation_mat * (glm::vec4(0, size.y, 0, 1)       - origin4), glm::vec2(0, 1)},
            vertex{position4 + rotation_mat * (glm::vec4(size.x, 0, 0, 1)       - origin4), glm::vec2(1, 0)},
            vertex{position4 + rotation_mat * (glm::vec4(0, size.y, 0, 1)       - origin4), glm::vec2(0, 1)},
            vertex{position4 + rotation_mat * (glm::vec4(size.x, 0, 0, 1)       - origin4), glm::vec2(1, 0)},
            vertex{position4 + rotation_mat * (glm::vec4(size.x, size.y, 0, 1)  - origin4), glm::vec2(1, 1)}
        };
    }
};