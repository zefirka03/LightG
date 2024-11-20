#include "obj_file.h"

std::vector<glm::vec3> objData::get_v() const {
    std::vector<glm::vec3> out;
    for (int i = 0; i < f.size(); ++i) {
        out.emplace_back(v[f[i].point[0].v_i - 1]);
        out.emplace_back(v[f[i].point[1].v_i - 1]);
        out.emplace_back(v[f[i].point[2].v_i - 1]);
    }
    return out;
}