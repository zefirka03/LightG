#pragma once
#include "renderer.h"

struct QuadRenderInstance : public RenderInstance {
    glm::vec3 position;
    glm::vec3 origin;
    glm::vec3 rotation;
    glm::vec2 size;

    QuadRenderInstance(
        glm::vec3 position, 
        glm::vec3 origin, 
        glm::vec3 rotation,
        glm::vec2 size
    ) : position(position), origin(origin), rotation(rotation), size(size) {}

    std::vector<vertex> get_vertices() const override {
        glm::mat4 rotation_mat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
        glm::vec4 origin4 = glm::vec4(origin, 0);
        glm::vec4 position4 = glm::vec4(position, 0);
        return {
            { position4 + ( -origin4 + glm::vec4(0, 0, 0, 0)          ) * rotation_mat, glm::vec2(0, 0) },
            { position4 + ( -origin4 + glm::vec4(0, size.y, 0, 0)     ) * rotation_mat, glm::vec2(0, 1) },
            { position4 + ( -origin4 + glm::vec4(size.x, 0, 0, 0)     ) * rotation_mat, glm::vec2(1, 0) },
            { position4 + ( -origin4 + glm::vec4(0, size.y, 0, 0)     ) * rotation_mat, glm::vec2(0, 1) },
            { position4 + ( -origin4 + glm::vec4(size.x, 0, 0, 0)     ) * rotation_mat, glm::vec2(1, 0) },
            { position4 + ( -origin4 + glm::vec4(size.x, size.y, 0, 0)) * rotation_mat, glm::vec2(1, 1) }
        };
    }
};