#pragma once
#include "air_engine.h"

struct chunkData {
    bool empty = true;
    std::vector<grassData> grass_data;
};

class MapManager : public Script {
public:
    void start(){

    }

    void update(float delta_time){
        if(!m_map_loaded) return;
    }

    void load_map_png(const char* path_folder, const char* name){
        if(m_map_loaded) return;

        std::string map_path = std::string(path_folder) + "/" + name;

        unsigned char* image = SOIL_load_image((map_path + "/" + name + ".png").c_str(), &size_x, &size_y, 0, SOIL_LOAD_RGBA);
        if(!image){
            printf("[MapManager] Load map failed!\n");
            return;
        }

        m_chunks = new chunkData[size_x * size_y]();
        for(int x = 0; x < size_x; ++x){
            for(int y = 0; y < size_y; ++y){
                if(image[4 * (y * size_x + x) + 3]){
                    m_chunks[y * size_x + x].empty = false;
                    _create_chunk_entity(x, y);
                    _load_grass_chuck(
                        (map_path + "/" + name + "_g_" + std::to_string(x) + "_" + std::to_string(y) + ".png").c_str(),
                        &m_chunks[y * size_x + x]
                    );
                }
            }
        }

        m_name = name;
        m_path = path_folder;
        m_map_loaded = true;
        SOIL_free_image_data(image);
    }

    ~MapManager(){
        printf("Map destructor called\n");
        if(m_map_loaded)
            delete[] m_chunks;
    }

private:
    void _reload(){

    }

    void _reload_chunks(){
        
    }

    void _create_chunk_entity(int x, int y){
        auto& scene = get_scene();
        auto& TM = get_scene().get_system<RenderingSystem>()->get_texture_manager();

        Entity plane11 = scene.create_entity();
        auto& sp_sp = scene.add_component<Sprite>(plane11);
        sp_sp.texture = TM.get_texture("default_1024");
        auto& sp_tr = scene.add_component<Transform>(plane11);
        auto& sp_pb = scene.add_component<PhysicsBody>(plane11);
        sp_pb.tag = 0;
        sp_pb.friction = 0.9f;
        sp_sp.size = glm::vec2(5000);
        sp_tr.origin = glm::vec3(0);
        sp_tr.rotation.x = glm::half_pi<float>();
        sp_tr.position = glm::vec3(sp_sp.size.x * x, 0, sp_sp.size.y * y);
        sp_pb.set_collider<PlaneCollider>();

        static_cast<PlaneCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
        static_cast<PlaneCollider*>(sp_pb.get_collider())->origin = glm::vec2(0);

        auto& rtx_draw = scene.add_component<RTX_Object>(plane11);
        rtx_draw.instance = new RTX_Plane(sp_tr.position, sp_tr.origin, sp_sp.size, 0);
    }

    void _load_grass_chuck(const char* file_path, chunkData* chunk){
        int szx, szy;
        unsigned char* image = SOIL_load_image(file_path, &szx, &szy, 0, SOIL_LOAD_RGBA);
        if (!image) {
            printf("[AIR][Grass system] file not found\n");
            return;
        }

        chunk->grass_data.clear();
        for (int x = 0; x < szx; ++x) {
            for (int z = 0; z < szy; ++z) {
                for (int g = 0; g < image[4 * (szx * z + x) + 3] / 5; ++g) {
                    chunk->grass_data.emplace_back(
                        x,
                        0,
                        z
                    );
                }
            }
        }
        SOIL_free_image_data(image);
    }

    const char* m_name = nullptr;
    const char* m_path = nullptr;

    int size_x = 0;
    int size_y = 0;
    chunkData* m_chunks = nullptr;
    bool m_map_loaded = false;
};