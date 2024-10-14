#pragma once
#include "ogl.h"
#include <vector>

class VAO{
private:
    struct bufferObject {
        GLuint id;
        GLuint type;
    };

    GLuint m_vao_id;
    std::vector<bufferObject> m_vbo_id;
    int m_vao_attributes = 0;
public:
    struct Index{
        GLuint buffer_index;
        GLint size; 
        GLsizei stride = 0; 
        const void* pointer = 0;
    };

    VAO();
    ~VAO();

    void bind();
    void unbind();
    void add_vbo(GLsizeiptr buffer_size);
    void add_ssbo(GLsizeiptr buffer_size, GLuint binding);
    void add_index(GLuint buffer_index, GLint size, GLsizei stride = 0, const void* pointer = 0);
    void add_index(VAO::Index const& index);
    void redata(GLuint buffer_index, int offset, int size, const void* data);
    void draw(int count, int type = GL_TRIANGLES, int first = 0);
};