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
    GLuint m_out_map_buffer;
    ComputeShader m_compute_shader;

    float m_time = 0;

    void init() override {
        map = new envField[size * size]();

        glGenBuffers(1, &m_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_map_buffer);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * size * sizeof(envField), map, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &m_out_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_out_map_buffer);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * size * sizeof(envField), map, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_out_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_compute_shader.load_from_file("shaders/environment_solver.shader");
    }
    
    void update(float delta_time) override {
        m_compute_shader.set_float(m_time, "time");
        m_compute_shader.use();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_map_buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * size * sizeof(envField), map);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_map_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_out_map_buffer);
        glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_out_map_buffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size* size *sizeof(envField), map);
        m_time += delta_time;
    }
};