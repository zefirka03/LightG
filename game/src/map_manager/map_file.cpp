#include "map_file.h"

MapFile::MapFile(const char* path){
    load_from_file(path);
}

int MapFile::load_from_file(const char* path){
    std::ifstream file(path);
    if(!file.is_open())
        return 0;

    std::string line;
    while(std::getline(file, line)) {
        std::istringstream stream(line);
        std::string prefix;

        stream >> prefix;
        if (prefix == "#") {
            continue;
        } else if(prefix == "path"){
            std::string tmp_name;
            stream >> tmp_name;

            std::string tmp_path;
            stream >> tmp_path;

            m_path[tmp_name] = tmp_path;
        }
    }
}

std::string MapFile::get(std::string name){
    auto it = m_path.find(name);
    if(it != m_path.end())
        return it->second;
    return "";
}

void MapFile::print_debug(){
    printf("--------- [MapFile] DEBUG ---------\n");
    for(auto& it : m_path){
        printf("%s : %s\n", it.first.c_str(), it.second.c_str());
    }
}