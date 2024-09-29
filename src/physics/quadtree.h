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
struct QuadNode {
    QuadNode* nodes = nullptr;
    std::vector<Quadable const*> childs;
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

    void add_child(Quadable const& child) {
        bounds.adjust_bounds(child.get_bounds());
        childs.push_back(&child);
    }

    void devide(){
        if (deep < AIR_MAX_DEEP && childs.size() >= AIR_QUAD_DEVIDE_SIZE) {
            int ppos = get_pool_position(pool_position);
            for (int i = 0; i < 8; ++i)
                *(nodes + ppos + i) = QuadNode(nodes, deep + 1, ppos + i);
            is_devided = true;

            int it = 0;
            for (int i = 0; i < childs.size(); ++i) {
                QuadNode* quad = get_quadrant(childs[i]->get_bounds());
                if (quad == this) childs[it++] = childs[i];
                else if (quad) quad->add_child(*childs[i]);
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
            glm::vec3 middle = (bounds.b + bounds.a) / 2.f;
            glm::vec3 subsize = (bounds.b - bounds.a) / 2.f;

            if (boundingBox(bounds.a, middle).contains(box))
                return nodes + ppos + 0;
            if (boundingBox(bounds.a + glm::vec3(subsize.x, 0, 0), middle + glm::vec3(subsize.x, 0, 0)).contains(box))
                return nodes + ppos + 1;
            if (boundingBox(bounds.a + glm::vec3(0, 0, subsize.z), middle + glm::vec3(0, 0, subsize.z)).contains(box))
                return nodes + ppos + 2;
            if (boundingBox(bounds.a + glm::vec3(subsize.x, 0, subsize.z), middle + glm::vec3(subsize.x, 0, subsize.z)).contains(box))
                return nodes + ppos + 3;
            if (boundingBox(bounds.a + glm::vec3(0, subsize.y, 0), middle + glm::vec3(0, subsize.y, 0)).contains(box))
                return nodes + ppos + 4;
            if (boundingBox(bounds.a + glm::vec3(subsize.x, subsize.y, 0), middle + glm::vec3(subsize.x, subsize.y, 0)).contains(box))
                return nodes + ppos + 5;
            if (boundingBox(bounds.a + glm::vec3(0, subsize.y, subsize.z), middle + glm::vec3(0, subsize.y, subsize.z)).contains(box))
                return nodes + ppos + 6;
            if (boundingBox(bounds.a + glm::vec3(subsize.x, subsize.y, subsize.z), middle + glm::vec3(subsize.x, subsize.y, subsize.z)).contains(box))
                return nodes + ppos + 7;

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

    void add_child(Quadable const& child) {
        m_nodes[0].add_child(child);
    }

    void draw_debug(DebugSystem& debug){
        m_nodes[0].draw_debug(debug);
    }

    void devide() {
        m_nodes[0].devide();
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