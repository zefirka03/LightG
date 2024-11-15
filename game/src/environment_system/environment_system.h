#pragma once
#include "core/core.h"

struct envField {
    float v_x = 0;
    float v_z = 0;
};

class EnvironmentSystem : public System {
public:
    int size = 128;
    envField* map = nullptr;
    GLuint m_out_map_buffer;

    envField* get_map(int x, int y) {
        if (x < 0 || y < 0 || x >= size || y >= size) {
            return nullptr;
        } return &map[x * size + y];
    }
private:
    GLuint m_map_buffer;
    GLuint m_map_back;
    ComputeShader m_compute_shader;
    bool m_map_reading = false;
    GLsync fence;

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
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * size * sizeof(envField), map, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_out_map_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &m_map_back);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_map_back);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * size * sizeof(envField), map, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_map_back);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_out_map_buffer);

        m_compute_shader.load_from_file("shaders/environment_solver_v2.shader");
    }
    
    void update(float delta_time) override {
        m_compute_shader.set_float(delta_time, "d_t");
        m_compute_shader.use();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_map_buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * size * sizeof(envField), map);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_map_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_out_map_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_map_back);
        glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_out_map_buffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * size * sizeof(envField), map);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_time += delta_time;
    }
        
};