#pragma once
#include "core/core.h"

struct envField {
    float x = 0;
    float z = 0;
};

class EnvironmentSystem : public System {
public:
    int size = 256;
    envField* map = nullptr;
private:
    GLuint m_map_buffer;
    ComputeShader m_compute_shader;

    void init() override {
        map = new envField[size * size]();
        glGenBuffers(1, &m_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_map_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_compute_shader.load_from_file("shaders/environment_solver.shader");
    }
    
    void update(float delta_time) override {

        m_compute_shader.use();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_map_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size*size*sizeof(envField), map, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_map_buffer);
        glDispatchCompute((size + 7) / 8, (size + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * size * sizeof(envField), map);
    }
};