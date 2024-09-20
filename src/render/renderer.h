#pragma once
#include "../core/core.h"
#include "camera_3d.h"

struct vertex {
    glm::vec3 position;
    glm::vec2 texture_coordinate;
};

template<class vertex_t>
class RenderInstance {
public:
    virtual std::vector<vertex_t> const& get_vertices() const = 0;
};

template<class vertex_t>
class Renderer{
private:
    std::vector<vertex_t> m_vertices;
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

    VAO& get_vao() {
        return m_vao;
    }

    void reserve(std::vector<VAO::Index> const& indices, size_t vertices_count){
        m_vertices.resize(vertices_count);
        m_vao.add_vbo(sizeof(vertex) * vertices_count);

        for(auto& index : indices)
            m_vao.add_index(index);

        m_vertices_count = 0;
    }

    void draw(std::vector<vertex_t> const& vertices) {
        for (int i = 0; i < vertices.size(); ++i)
            m_vertices[m_vertices_count++] = vertices[i];
    }

    void draw(RenderInstance<vertex_t> const& instance) {
        draw(instance.get_vertices());
    }

    void display(int object_type = GL_TRIANGLES) {
        m_shader.use();

        m_vao.redata(0, 0, m_vertices_count * sizeof(vertex_t), m_vertices.data());
        m_vao.draw(m_vertices_count, object_type);
        m_vertices_count = 0;

        m_shader.unuse();
    }
};