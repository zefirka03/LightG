#include "obj_file.h"


objFile::objFile() {

}

objFile::objFile(const char* path) {
    load_from_file(path);
}

objFile::~objFile(){

}

void objFile::load_from_file(const char* path){
    std::ifstream file(path);
    if(!file.is_open()){
        printf("[AIR][objFile]: Something went wrong. File not opened\n");
        return;
    }
    
    objData* curr_data = nullptr;
    std::string line;
    while(std::getline(file, line)) {
        std::istringstream stream(line);
        std::string prefix;

        stream >> prefix;
        if (prefix == "o") {
            std::string name;
            stream >> name;

            m_objects[name] = objData();
            curr_data = &m_objects.at(name);
            curr_data->name = name;
        } else if (prefix == "v") {
            glm::vec3 v;
            stream >> v.x >> v.y >> v.z;
            curr_data->v.emplace_back(v);
        } else if (prefix == "vt") {
            glm::vec3 vt;
            stream >> vt.x >> vt.y;
            curr_data->vt.emplace_back(vt);
        } else if (prefix == "vn") {
            glm::vec3 vn;
            stream >> vn.x >> vn.y >> vn.z;
            curr_data->vn.emplace_back(vn);
        } else if (prefix == "f") {
            objData::Face face;

            std::string token;
            for (int p = 0; p < 3; ++p) {
                stream >> token;

                size_t pos1 = token.find('/');
                size_t pos2 = token.find('/', pos1 + 1);

                if (pos1 != std::string::npos)
                    face.point[p].v_i = std::stoi(token.substr(0, pos1));

                if (pos1 != std::string::npos && pos2 != std::string::npos)
                    face.point[p].vt_i = std::stoi(token.substr(pos1 + 1, pos2 - pos1 - 1));
                else face.point[p].vt_i = 0; 

                if (pos2 != std::string::npos)
                    face.point[p].vn_i = std::stoi(token.substr(pos2 + 1));
                else face.point[p].vn_i = 0; 

            }
            curr_data->f.emplace_back(face);
        }
    }

    file.close();
}

void objFile::print() {
    for(auto& object : m_objects){
        printf("Object: %s\n", object.second.name.c_str());
        for(auto& v : object.second.v)
            printf("\bv: %f %f %f\n", v.x, v.y, v.z);

        for(auto& vt : object.second.vt)
            printf("\bvt: %f %f\n", vt.x, vt.y);

        for(auto& vn : object.second.vn)
            printf("\bvn: %f %f %f\n", vn.x, vn.y, vn.z);

        for(auto& f : object.second.f){
            printf("-----\n");
            for(int p = 0; p < 3; ++p)
                printf("f: %ld %ld %ld\n", f.point[p].v_i, f.point[p].vt_i, f.point[p].vn_i);
            printf("-----\n");
        }
    }
}

objData& objFile::operator[](std::string _name){
    return m_objects[_name];
}