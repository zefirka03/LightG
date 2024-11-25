#include "gpu_vector_storage.h"

GPUVectorStorage::GPUVectorStorage(){
    glGenBuffers(1, &m_ssbo);
    _resize(m_capacity);
}

void GPUVectorStorage::push_back(const void* data, size_t size){
    while(m_capacity < m_size + size){
        _resize(m_capacity << 1);
    }
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_size, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_size += size;
}

void GPUVectorStorage::clear(){
    m_size = 0;
}

GLuint GPUVectorStorage::get_ssbo() const {
    return m_ssbo;
}

void GPUVectorStorage::bind_base(int index) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ssbo);
}

void GPUVectorStorage::_resize(size_t size) {
    GLuint newSSBO; 
    glGenBuffers(1, &newSSBO); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO); 
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_STREAM_DRAW); 

    if (m_size > 0) { 
        glBindBuffer(GL_COPY_READ_BUFFER, m_ssbo);
        glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_size);
    }

    glDeleteBuffers(1, &m_ssbo); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); 

    m_ssbo = newSSBO; 
    m_capacity = size;
}

size_t GPUVectorStorage::size() const {
    return m_size;
}

GPUVectorStorage::~GPUVectorStorage(){
    glDeleteBuffers(1, &m_ssbo);
}