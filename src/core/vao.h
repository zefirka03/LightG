#pragma once
#include "ogl.h"
#include <vector>

class VAO{
private:
    GLuint m_vao_id;
    std::vector<GLuint> m_vbo_id;
    int m_vao_attributes = 0;
public:
    VAO();
    ~VAO();

    void bind();
    void unbind();
    void addVBO(GLsizeiptr buffer_size);
    void addVAO(GLuint vbo_index, GLint vao_size, GLsizei stride = 0, const void* pointer = 0);
    void redata(int vbo_index, int offset, int size, const void* data);
    void draw(int count);
};