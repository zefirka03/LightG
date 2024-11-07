#include "vao.h"

VAO::VAO(){
    glGenVertexArrays(1, &m_vao_id);
}

VAO::~VAO(){
    glDeleteVertexArrays(1, &m_vao_id);

    for (int i = 0; i < m_vbo_id.size(); ++i)
        glDeleteBuffers(1, &m_vbo_id[i].id);
}

void VAO::bind(){
    glBindVertexArray(m_vao_id);
}

void VAO::unbind(){
    glBindVertexArray(0);
}

void VAO::add_vbo(GLsizeiptr buffer_size){
    bind();
    GLuint curr_vbo;
    glGenBuffers(1, &curr_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, curr_vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
    m_vbo_id.push_back({ curr_vbo, GL_ARRAY_BUFFER});
    unbind();
}

void VAO::add_ssbo(GLsizeiptr buffer_size, GLuint binding) {
    bind();
    GLuint curr_ssbo;
    glGenBuffers(1, &curr_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, curr_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, curr_ssbo);
    m_vbo_id.push_back({ curr_ssbo, GL_SHADER_STORAGE_BUFFER });
    unbind();
}

void VAO::add_index(GLuint buffer_index, GLint size, GLsizei stride, const void* pointer) {
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id[buffer_index].id);

	glVertexAttribPointer(m_vao_attributes, size, GL_FLOAT, GL_FALSE, stride, pointer);
    glEnableVertexAttribArray(m_vao_attributes++);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbind();
}

void VAO::add_index(VAO::Index const& index){
    add_index(index.buffer_index, index.size, index.stride, index.pointer);
}

void VAO::redata(GLuint bo_index, int offset, int size, const void* data){
    bind();
    GLuint id = m_vbo_id[bo_index].id;
    GLuint type = m_vbo_id[bo_index].type;
    glBindBuffer(type, id);
    glBufferSubData(type, offset, size, data);
    unbind();
}

void VAO::draw(int count, int type, int first) {
	bind();
	glDrawArrays(type, first, count);
	unbind();
}