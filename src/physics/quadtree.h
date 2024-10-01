 #pragma once
#include <vector>
#include "../debug/debug.h"
#include "bounding_box.h"

#define AIR_QUAD_DEVIDE_SIZE 8
#define AIR_MAX_DEEP 3

struct Quadable {
    virtual boundingBox get_bounds() const = 0;
};

/// 
/// First you add all childs to root node, then do devide
/// 
class QuadNode {
private:
    boundingBox m_subbounds_cache[8];

    void _update_subbounds_cache() {
        glm::vec3 middle = (bounds.b + bounds.a) / 2.f;
        glm::vec3 subsize = (bounds.b - bounds.a) / 2.f;

        m_subbounds_cache[0] = boundingBox(bounds.a, middle);
        m_subbounds_cache[1] = boundingBox(bounds.a + glm::vec3(subsize.x, 0, 0), middle + glm::vec3(subsize.x, 0, 0));
        m_subbounds_cache[2] = boundingBox(bounds.a + glm::vec3(0, 0, subsize.z), middle + glm::vec3(0, 0, subsize.z));
        m_subbounds_cache[3] = boundingBox(bounds.a + glm::vec3(subsize.x, 0, subsize.z), middle + glm::vec3(subsize.x, 0, subsize.z));
        m_subbounds_cache[4] = boundingBox(bounds.a + glm::vec3(0, subsize.y, 0), middle + glm::vec3(0, subsize.y, 0));
        m_subbounds_cache[5] = boundingBox(bounds.a + glm::vec3(subsize.x, subsize.y, 0), middle + glm::vec3(subsize.x, subsize.y, 0));
        m_subbounds_cache[6] = boundingBox(bounds.a + glm::vec3(0, subsize.y, subsize.z), middle + glm::vec3(0, subsize.y, subsize.z));
        m_subbounds_cache[7] = boundingBox(bounds.a + glm::vec3(subsize.x, subsize.y, subsize.z), middle + glm::vec3(subsize.x, subsize.y, subsize.z));
    }
public:
    QuadNode* nodes = nullptr;
    std::vector<Quadable*> childs;
    boundingBox bounds = boundingBox();
    bool is_devided = false;
    int deep = 0;
    int pool_position = 0;

    QuadNode() {
        childs.reserve(AIR_QUAD_DEVIDE_SIZE);
    }

    QuadNode(QuadNode* _node, int _deep, int _pool_position) : nodes(_node), deep(_deep), pool_position(_pool_position) {
        childs.reserve(AIR_QUAD_DEVIDE_SIZE);
    }

    void add_child(Quadable* child) {
        bounds.adjust_bounds(child->get_bounds());
        childs.emplace_back(child);
    }

    void devide(){
        if (deep < AIR_MAX_DEEP && childs.size() >= AIR_QUAD_DEVIDE_SIZE) {
            int ppos = get_pool_position(pool_position);
            for (int i = 0; i < 8; ++i)
                *(nodes + ppos + i) = QuadNode(nodes, deep + 1, ppos + i);
            is_devided = true;

            int it = 0;
            _update_subbounds_cache();
            for (int i = 0; i < childs.size(); ++i) {
                QuadNode* quad = get_quadrant(childs[i]->get_bounds());
                if (quad == this) childs[it++] = childs[i];
                else if (quad) quad->add_child(childs[i]);
            }
            childs.resize(it);

            for (int i = 0; i < 8; ++i)
                (nodes + ppos + i)->devide();
        }
    }

    int get_pool_position(int i){
        return 8 * i + 1;
    }

    QuadNode* get_quadrant(boundingBox const& box) {
        int ppos = get_pool_position(pool_position);
        if (bounds.contains(box)) {
            for (int i = 0; i < 8; ++i)
                if (m_subbounds_cache[i].contains(box))
                    return nodes + ppos + i;

            return this;
        }
        return nullptr;
    }

    void draw_debug(DebugSystem& debug){
        debug.draw_box(bounds.a, bounds.b, glm::vec4(0, float(deep + 1) / float(AIR_MAX_DEEP + 1), float(deep + 1) / float(AIR_MAX_DEEP + 1), 1));
        if(is_devided){
            int ppos = get_pool_position(pool_position);
            for(int i = 0; i < 8; ++i)
                (nodes + ppos + i)->draw_debug(debug);
        }
    }

    std::vector<Quadable*> const& get_colliders(Quadable* quad) {
        if (is_devided) {
            QuadNode* tmp = get_quadrant(quad->get_bounds());
            if (tmp == this) return childs;
            if(tmp) return get_quadrant(quad->get_bounds())->get_colliders(quad);
            return std::vector<Quadable*>(0);
        }
        else return childs;
    }

    ~QuadNode(){}
};

class Quadtree {
private:
    QuadNode* m_nodes;
    int m_size;
public:
    Quadtree() {
        m_size = (2 << (3 * (AIR_MAX_DEEP + 1)) - 1) / 7;
        m_nodes = new QuadNode[m_size]();

        m_nodes[0].nodes = m_nodes;
        m_nodes[0].deep = 0;
        m_nodes[0].pool_position = 0;
    }

    void add_child(Quadable* child) {
        m_nodes[0].add_child(child);
    }

    void draw_debug(DebugSystem& debug){
        m_nodes[0].draw_debug(debug);
    }

    void devide() {
        m_nodes[0].devide();
    }

    std::vector<Quadable*> const& get(Quadable* quad) {
        return m_nodes[0].get_colliders(quad);
    }

    void clear() {
        for (int i = 0; i < m_size; ++i) {
            m_nodes[i].childs.clear();
            m_nodes[i].is_devided = false;
            m_nodes[i].bounds = boundingBox();
        }
    }

    ~Quadtree() {
        delete[] m_nodes;
    }
};