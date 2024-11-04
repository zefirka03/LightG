#pragma once
#include "ogl.h"
#include "log.h"

#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <string>

class ComputeShader {
private:
    std::unordered_map<std::string_view, GLuint> m_locations_cache;
    GLuint m_prog_id = 0;
    bool m_inited;

    GLuint _request_location(const char* path);
    void _destroy();
public:
    ComputeShader();
    ComputeShader(const char* path);
    ~ComputeShader();

    void use();
    void unuse();
    void load_from_file(const char* path);
    void load_from_string(const char* string);
    bool is_inited();
    GLuint get_id();

    void set_matrix4f(glm::mat4 val, const char* path);
    void set_vector2f(glm::vec2 val, const char* path);
    void set_vector3f(glm::vec3 val, const char* path);
    void set_vector4f(glm::vec4 val, const char* path);
    void set_float(GLfloat val, const char* path);
};