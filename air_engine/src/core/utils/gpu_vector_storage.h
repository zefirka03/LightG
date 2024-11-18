#pragma once
#include "../ogl.h"

#define _AIR_GPU_VECTOR_INIT_CAPACITY 1024

class GPUVectorStorage {
public:
    GPUVectorStorage();
    void push_back(const void* data, size_t size);
    void clear();
    GLuint get_ssbo() const;
    void bind_base(int base) const;
    size_t size() const;
    ~GPUVectorStorage();
private:
    void _resize(size_t size);

    size_t m_size = 0;
    size_t m_capacity = _AIR_GPU_VECTOR_INIT_CAPACITY;
    GLuint m_ssbo = 0;
};