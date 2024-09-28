#pragma once
#include <vector>
#include "../debug/debug.h"
#include "bounding_box.h"

#define AIR_QUAD_DEVIDE_SIZE 8
#define AIR_MAX_DEEP 2

struct Quadable {
    virtual boundingBox get_bounds() const = 0;
};


/// 
/// First you add all childs to root node, then do devide
/// 
struct QuadNode {
    std::vector<QuadNode*> nodes;
    std::vector<Quadable const*> childs;
    boundingBox bounds = boundingBox();
    bool is_devided = false;
    int deep = 0;

    QuadNode() {
        nodes.resize(8, nullptr);
        childs.reserve(AIR_QUAD_DEVIDE_SIZE);
    }

    QuadNode(int _deep) : deep(_deep) {
        nodes.resize(8, nullptr);
        childs.reserve(AIR_QUAD_DEVIDE_SIZE);
    }

    void add_child(Quadable const& child) {
        bounds.adjust_bounds(child.get_bounds());
        childs.push_back(&child);
    }

    void devide(){
        if (deep < AIR_MAX_DEEP && childs.size() >= AIR_QUAD_DEVIDE_SIZE) {
            for (int i = 0; i < nodes.size(); ++i)
                nodes[i] = new QuadNode(deep + 1);
            is_devided = true;

            int it = 0;
            for (int i = 0; i < childs.size(); ++i) {
                QuadNode* quad = get_quadrant(childs[i]->get_bounds());
                if (quad == this) childs[it++] = childs[i];
                else if (quad) quad->add_child(*childs[i]);
            }
            childs.resize(it);

            for (int i = 0; i < nodes.size(); ++i)
                nodes[i]->devide();
        }
    }

    QuadNode* get_quadrant(boundingBox const& box) {
        if (bounds.contains(box)) {
            glm::vec3 middle = (bounds.b + bounds.a) / 2.f;
            glm::vec3 subsize = (bounds.b - bounds.a) / 2.f;

            if (boundingBox(bounds.a, middle).contains(box))
                return nodes[0];
            if (boundingBox(bounds.a + glm::vec3(subsize.x, 0, 0), middle + glm::vec3(subsize.x, 0, 0)).contains(box))
                return nodes[1];
            if (boundingBox(bounds.a + glm::vec3(0, 0, subsize.z), middle + glm::vec3(0, 0, subsize.z)).contains(box))
                return nodes[2];
            if (boundingBox(bounds.a + glm::vec3(subsize.x, 0, subsize.z), middle + glm::vec3(subsize.x, 0, subsize.z)).contains(box))
                return nodes[3];
            if (boundingBox(bounds.a + glm::vec3(0, subsize.y, 0), middle + glm::vec3(0, subsize.y, 0)).contains(box))
                return nodes[4];
            if (boundingBox(bounds.a + glm::vec3(subsize.x, subsize.y, 0), middle + glm::vec3(subsize.x, subsize.y, 0)).contains(box))
                return nodes[5];
            if (boundingBox(bounds.a + glm::vec3(0, subsize.y, subsize.z), middle + glm::vec3(0, subsize.y, subsize.z)).contains(box))
                return nodes[6];
            if (boundingBox(bounds.a + glm::vec3(subsize.x, subsize.y, subsize.z), middle + glm::vec3(subsize.x, subsize.y, subsize.z)).contains(box))
                return nodes[7];

            return this;
        }
        return nullptr;
    }

    void draw_debug(DebugSystem& debug){
        debug.draw_box(bounds.a, bounds.b, glm::vec4(0, float(deep + 1) / float(AIR_MAX_DEEP + 1), float(deep + 1) / float(AIR_MAX_DEEP + 1), 1));
        if(is_devided){
            for(int i = 0; i < nodes.size(); ++i)
                nodes[i]->draw_debug(debug);
        }
    }

    ~QuadNode(){
        for(int i = 0; i < nodes.size(); ++i)
            delete nodes[i];
    }
};

class Quadtree {
private:
    QuadNode* m_nodes;
public:
    Quadtree() {
        m_nodes = new QuadNode[std::pow(8, AIR_MAX_DEEP + 1) / 7];
    }

    void add_child(Quadable const& child) {
        m_nodes[0].add_child(child);
    }
};