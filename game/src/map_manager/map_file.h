#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <string_view>
#include <unordered_map>

class MapFile{
public:
    MapFile(const char* path);
    int load_from_file(const char* path);
    std::string get(std::string name);
    void print_debug();

private:
    bool m_is_opened = false;
    std::unordered_map<std::string, std::string> m_path;
};