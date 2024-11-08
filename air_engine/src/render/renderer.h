#pragma once
#include "../core/core.h"
#include "camera_3d.h"
#include <array>

template<class vertex_t, size_t count>
class RenderInstance {
public:
    static size_t get_count() {
        return count;
    }

    virtual std::array<vertex_t, count> get_vertices() const = 0;
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
        m_vao.add_vbo(sizeof(vertex_t) * vertices_count);

        for(auto& index : indices)
            m_vao.add_index(index);

        m_vertices_count = 0;
    }
    
    template<std::size_t size>
    void draw(std::array<vertex_t, size> const& vertices) {
        for (int i = 0; i < vertices.size(); ++i)
            m_vertices[m_vertices_count++] = vertices[i];
    }

    template<std::size_t size>
    void draw(RenderInstance<vertex_t, size> const& instance) {
        draw(instance.get_vertices());
    }

    void display(int object_type = GL_TRIANGLES) {
        m_shader.use();

        m_vao.redata(0, 0, m_vertices_count * sizeof(vertex_t), m_vertices.data());
        m_vao.draw(m_vertices_count, object_type);
        m_vertices_count = 0;

        m_shader.unuse();
    }

    void load_all() {
        m_vao.redata(0, 0, m_vertices_count * sizeof(vertex_t), m_vertices.data());
    }

    void display_part(int object_type, int count, int first) {
        m_shader.use();
        m_vao.draw(count, object_type, first);
        m_shader.unuse();
    }

    void clear() {
        m_vertices_count = 0;
    }

    std::vector<vertex_t>& get_vertices() const {
        return m_vertices;
    }
};