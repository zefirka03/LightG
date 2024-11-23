#pragma once
#include <algorithm>

#include "air_engine.h"
#include "map_file.h"
#include "../grass_system/grass_system.h"

struct chunkData {
    bool empty = true;
    std::vector<grassData> grass_data;

    glm::vec3 position;
    double distance = FLT_MAX;
};

class MapManager : public Script {
public:
    void start(){

    }

    void update(float delta_time){
        if(!m_map_loaded) return;
        
        std::vector<Entity> camera_entities = get_scene().get_all_entities_of_type<Camera3d>();
        Entity camera_entity;
        for (auto& it : camera_entities) {
            if (get_scene().get_component<Camera3d>(it).is_main()) {
                camera_entity = it;
                break;
            }
        }

        auto& camera = get_scene().get_component<Transform>(camera_entity);

        if (glm::length(camera.position - m_last_camera_position) > m_min_camera_displace) {
            printf("UPDATE\n");
            m_min_camera_displace = FLT_MAX;
            m_last_camera_position = camera.position;
            for (int i = 0; i < m_size_x * m_size_y; ++i) {
                m_chunks[i].distance = glm::length(camera.position - m_chunks[i].position);
                m_min_camera_displace = std::min(m_min_camera_displace, abs(float(m_chunks[i].distance) - 25000.f));
            }

            _update_grass_chunks();
        }
    }

    void load_map_png(const char* path){
        if(m_map_loaded) return;

        MapFile mf(path);
        mf.print_debug();

        unsigned char* image = SOIL_load_image(mf.get("<map_pl>").c_str(), &m_size_x, &m_size_y, 0, SOIL_LOAD_RGBA);
        if(!image){
            printf("[MapManager] Load map failed!\n");
            return;
        }

        if (!m_textures.load_texture(mf.get("<map_tex>").c_str(), "map_texture")) {
            printf("[MapManager] Texture of map not found! Skip.\n");
        }

        auto GS = get_scene().get_system<GrassSystem>();
        GS->clear();
        
        std::string grass_path = mf.get("<map_gr>");
        m_chunks = new chunkData[m_size_x * m_size_y]();
        for(int x = 0; x < m_size_x; ++x){
            for(int y = 0; y < m_size_y; ++y){
                m_chunks[y * m_size_x + x].position = glm::vec3((x + 0.5f) * m_chunk_size, 0, (y + 0.5f) * m_chunk_size);
                if(image[4 * (y * m_size_x + x) + 3]){
                    m_chunks[y * m_size_x + x].empty = false;

                    _create_chunk_entity(x, y);
                    _load_grass_chuck(
                        (grass_path + "_" + std::to_string(x) + "_" + std::to_string(y) + ".png").c_str(),
                        x, y
                    );
                }
            }
        }
        
        m_path = path;
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

    void _update_grass_chunks() {
        std::sort(m_chunks, m_chunks + m_size_x * m_size_y, [](chunkData const& a, chunkData const& b) -> bool {
            return a.distance < b.distance;
            });

        auto GS = get_scene().get_system<GrassSystem>();
        GS->clear();
        for (int i = 0; i < m_size_x * m_size_y; ++i) {
            auto& curr_chunk = m_chunks[i];
            if (m_chunks[i].distance < 25000) {
                GS->push_grass(
                    m_chunks[i].grass_data,
                    boundingBox(
                        curr_chunk.position - glm::vec3(0.5f * m_chunk_size, 0, 0.5f * m_chunk_size),
                        curr_chunk.position + glm::vec3(0.5f * m_chunk_size, 0, 0.5f * m_chunk_size)
                    )
                );
            }
        }
    }

    void _create_chunk_entity(int x, int y){
        auto& scene = get_scene();
        auto& TM = get_scene().get_system<RenderingSystem>()->get_texture_manager();

        Entity plane11 = scene.create_entity();
        auto& sp_sp = scene.add_component<Sprite>(plane11);

        auto map_texture = m_textures.get_texture("map_texture");
        if(map_texture){
            sp_sp.texture = map_texture;
            sp_sp.texture_rect = glm::vec4(
                x / float(m_size_x), y / float(m_size_y),
                (x + 1) / float(m_size_x), (y + 1) / float(m_size_y)
            );
        } else {
            sp_sp.texture = TM.get_texture("default_1024");
        } 

        auto& sp_tr = scene.add_component<Transform>(plane11);
        auto& sp_pb = scene.add_component<PhysicsBody>(plane11);
        sp_pb.tag = 0;
        sp_pb.friction = 0.9f;
        sp_sp.size = glm::vec2(m_chunk_size);
        sp_tr.origin = glm::vec3(0);
        sp_tr.rotation.x = glm::half_pi<float>();
        sp_tr.position = glm::vec3(sp_sp.size.x * x, 0, sp_sp.size.y * y);
        sp_pb.set_collider<PlaneCollider>();

        static_cast<PlaneCollider*>(sp_pb.get_collider())->size = glm::vec2(sp_sp.size);
        static_cast<PlaneCollider*>(sp_pb.get_collider())->origin = glm::vec2(0);

        auto& rtx_draw = scene.add_component<RTX_Object>(plane11);
        rtx_draw.instance = new RTX_Plane(sp_tr.position, sp_tr.origin, sp_sp.size, 0);
    }

    void _load_grass_chuck(const char* file_path, int ch_x, int ch_y){
        int szx, szy;
        unsigned char* image = SOIL_load_image(file_path, &szx, &szy, 0, SOIL_LOAD_RGBA);
        if (!image) {
            //printf("[AIR][Grass system] file not found\n");
            return;
        }

        auto chunk = &m_chunks[ch_y * m_size_x + ch_x];

        float disp_x = ch_x * m_chunk_size;
        float disp_z = ch_y * m_chunk_size;
        float block_sz_x = m_chunk_size / float(szx);
        float block_sz_z = m_chunk_size / float(szy);

        chunk->grass_data.clear();
        for (int x = 0; x < szx; ++x) {
            for (int z = 0; z < szy; ++z) {
                for (int g = 0; g < image[4 * (szx * z + x) + 3] / 2; ++g) {
                    chunk->grass_data.emplace_back(
                        disp_x + (szx - 1 - x + (rand() % 10000) / 10000.f) * block_sz_x,
                        0,
                        disp_z + (szy - 1 - z + (rand() % 10000) / 10000.f) * block_sz_z
                    );
                }
            }
        }
        SOIL_free_image_data(image);
    }

    TextureManager m_textures;

    const char* m_path = nullptr;

    int m_size_x = 0;
    int m_size_y = 0;
    float m_chunk_size = 25000;
    chunkData* m_chunks = nullptr;

    bool m_map_loaded = false;

    float m_min_camera_displace = 0;
    glm::vec3 m_last_camera_position = glm::vec3(FLT_MAX);
};