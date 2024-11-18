#include "gpu_vector_storage.h"

GPUVectorStorage::GPUVectorStorage(){
    glGenBuffers(1, &m_ssbo);
}

void GPUVectorStorage::push_back(void* data, size_t size){
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

void GPUVectorStorage::_resize(size_t size) {
    if(m_capacity < size){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        m_capacity = size;
    }
}

GPUVectorStorage::~GPUVectorStorage(){

}