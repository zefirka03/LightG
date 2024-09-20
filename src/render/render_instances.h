#pragma once
#include "renderer.h"

struct QuadRenderInstance : public RenderInstance<vertex> {
private:
    static std::vector<vertex> m_cached_verteces;

public:
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

    std::vector<vertex> const& get_vertices() const override {
        glm::mat4 rotation_mat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
        glm::vec4 origin4 = glm::vec4(origin, 0);
        glm::vec4 position4 = glm::vec4(position, 0);

        m_cached_verteces[0] = {position4 + ( -origin4 + glm::vec4(0, 0, 0, 0)          ) * rotation_mat, glm::vec2(0, 0)};
        m_cached_verteces[1] = {position4 + ( -origin4 + glm::vec4(0, size.y, 0, 0)     ) * rotation_mat, glm::vec2(0, 1)};
        m_cached_verteces[2] = {position4 + ( -origin4 + glm::vec4(size.x, 0, 0, 0)     ) * rotation_mat, glm::vec2(1, 0)};
        m_cached_verteces[3] = {position4 + ( -origin4 + glm::vec4(0, size.y, 0, 0)     ) * rotation_mat, glm::vec2(0, 1)};
        m_cached_verteces[4] = {position4 + ( -origin4 + glm::vec4(size.x, 0, 0, 0)     ) * rotation_mat, glm::vec2(1, 0)};
        m_cached_verteces[5] = {position4 + ( -origin4 + glm::vec4(size.x, size.y, 0, 0)) * rotation_mat, glm::vec2(1, 1)};

        return m_cached_verteces;
    }
};

std::vector<vertex> QuadRenderInstance::m_cached_verteces = std::vector<vertex>(6);