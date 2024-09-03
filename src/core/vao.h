#pragma once
#include "ogl.h"
#include <vector>

class VAO{
private:
    GLuint m_vao_id;
    std::vector<GLuint> m_vbo_id;
public:
    VAO();
    ~VAO();

    void bind();
    void unbind();
    void addVBO(GLsizeiptr buffer_size, GLint vao_size);
    void redata(int vbo_index, int offset, int size, const void* data);
    void draw(int count);
};