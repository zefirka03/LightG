#pragma once
#include "../core/air_engine.h"
#include "../core/shader.h"
#include "camera_3d.h"

struct vertex {
    glm::vec3 position;
    glm::vec2 texture_coordinate;
};

class RenderInstance {
public:
    virtual std::vector<vertex> get_vertices() const = 0;
};

class Renderer{
private:
    std::vector<vertex> m_vertices;
    size_t m_vertices_count;
    VAO m_vao;
    Shader m_shader;

    void _default_shader_load() {
        m_shader.load_from_file(
            "shaders/default.shader",
            AIR_SHADER_VF
        );
    }
public:
    Renderer() {
        _default_shader_load();
    };
    ~Renderer() {}

    Shader& get_shader() {
        return m_shader;
    }

    void reserve(size_t vertices_count){
        m_vertices.resize(vertices_count);
        m_vao.addVBO(sizeof(vertex) * vertices_count);
        m_vao.addVAO(0, 3, sizeof(vertex), (const void*)0);
        m_vao.addVAO(0, 2, sizeof(vertex), (const void*)sizeof(glm::vec3));

        m_vertices_count = 0;
    }

    void draw(std::vector<vertex> const& vertices) {
        for (int i = 0; i < vertices.size(); ++i)
            m_vertices[m_vertices_count++] = vertices[i];
    }

    void draw(RenderInstance const& instance) {
        draw(instance.get_vertices());
    }

    void display() {
        m_shader.use();

        m_vao.redata(0, 0, m_vertices_count * sizeof(vertex), m_vertices.data());
        m_vao.draw(m_vertices_count);
        m_vertices_count = 0;

        m_shader.unuse();
    }
};