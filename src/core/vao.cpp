#include "vao.h"

VAO::VAO(){
    glGenVertexArrays(1, &m_vao_id);
}

VAO::~VAO(){
    glDeleteVertexArrays(1, &m_vao_id);
}

void VAO::bind(){
    glBindVertexArray(m_vao_id);
}

void VAO::unbind(){
    glBindVertexArray(0);
}

void VAO::addVBO(GLsizeiptr buffer_size, GLint vao_size){
    bind();
    GLuint curr_vbo;
    glGenBuffers(1, &curr_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, curr_vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(m_vbo_id.size(), vao_size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(m_vbo_id.size());

    m_vbo_id.emplace_back(curr_vbo);
    unbind();
}

void VAO::redata(int vbo_index, int offset, int size, const void* data){
    bind();
    GLuint vbo_id = m_vbo_id[vbo_index];
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    unbind();
}

void VAO::draw(int count) {
	bind();
	glDrawArrays(GL_TRIANGLES, 0, count);
	unbind();
}