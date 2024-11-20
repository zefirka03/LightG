#pragma once
#include "../ogl.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <string_view>

struct objData {
    std::string name;

    struct Face {
        struct point{
            size_t v_i;
            size_t vt_i;
            size_t vn_i;
        } point[3];
    };
    // vertices
    std::vector<glm::vec3> v;
    // texture coords
    std::vector<glm::vec2> vt;
    // normals
    std::vector<glm::vec3> vn;
    // faces {v_index, vt_index, vn_index}
    std::vector<Face> f;

    std::vector<glm::vec3> get_v() const;
};

class objFile {
public:
    objFile();
    objFile(const char* path);
    ~objFile();

    void load_from_file(const char* path);
    void print();
    objData& operator[](std::string _name);
private:
    std::unordered_map<std::string, objData> m_objects;
};