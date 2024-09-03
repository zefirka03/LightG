#pragma once
#include "ogl.h"
#include "log.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <string>

#define AIR_SHADER_VF 0
#define AIR_SHADER_VGF 1

class Shader {
private:
    std::unordered_map<std::string_view, GLuint> m_locations_cache;
    GLuint m_prog_id = 0;
    bool m_inited;

    GLuint _request_location(const char* path);
public:
    Shader();
    // usings - AIR_SHADER_VF or AIR_SHADER_VGF (use VGF when you using geometry shader)
    Shader(const char* path, uint8_t usings);
    ~Shader();

    void use();
    void unuse();
    // usings - AIR_SHADER_VF or AIR_SHADER_VGF (use VGF when you using geometry shader)
    void loadFromFile(const char* path, uint8_t usings);
    // usings - AIR_SHADER_VF or AIR_SHADER_VGF (use VGF when you using geometry shader)
    void loadFromString(const char* string, uint8_t usings);
    bool isInited();
    GLuint getId();

    void setMatrix4f(glm::mat4 val, const char* path);
    void setVector2f(glm::vec2 val, const char* path);
    void setVector4f(glm::vec4 val, const char* path);
    void setFloat(GLfloat val, const char* path);
};