#pragma once
#include <vector>
#include "../debug/debug.h"
#include "bounding_box.h"

#define AIR_QUAD_DEVIDE_SIZE 16
#define AIR_MAX_DEEP 2

struct Quadable {
    virtual boundingBox get_bounds() const = 0;
};

/// 
/// First you add all childs to root node, then do devide
/// 
class QuadNode {
private:
    void _collect_intersections(boundingBox const& box, std::vector<Quadable*>& array) const {
        // this quadnode
        if(box.intersect(bounds)){
            for(auto child : childs)
                array.emplace_back(child);

            // other
            if (is_devided) {
                int ppos = get_pool_position(pool_position);
                for (int i = 0; i < 8; ++i)
                    (nodes + ppos + i)->_collect_intersections(box, array);
            }
        }
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
            for (int i = 0; i < childs.size(); ++i) {
                int quadrant = get_quadrant(childs[i]->get_bounds());
                if (quadrant == -1) childs[it++] = childs[i];
                else (nodes + ppos + quadrant)->add_child(childs[i]);
            }
            childs.resize(it);

            for (int i = 0; i < 8; ++i)
                (nodes + ppos + i)->devide();
        }
    }

    int get_pool_position(int i) const {
        return 8 * i + 1;
    }

    int get_quadrant(boundingBox const& box) {
        glm::vec3 middle = (bounds.a + bounds.b) / 2.f;

        if(box.a.x > middle.x){
            if(box.a.y > middle.y){
                if(box.a.z > middle.z){
                    return 0;
                }
                else if(box.b.z <= middle.z){
                    return 1;
                }
            }else if(box.b.y <= middle.y){
                if(box.a.z > middle.z){
                    return 2;
                }
                else if(box.b.z <= middle.z){
                    return 3;
                }
            }
        }else if(box.b.x <= middle.x){
            if(box.a.y > middle.y){
                if(box.a.z > middle.z){
                    return 4;
                }
                else if(box.b.z <= middle.z){
                    return 5;
                }
            }else if(box.b.y <= middle.y){
                if(box.a.z > middle.z){
                    return 6;
                }
                else if(box.b.z <= middle.z){
                    return 7;
                }
            }
        }

        return -1;
    }

    void draw_debug(DebugSystem& debug){
        debug.draw_box(bounds.a, bounds.b, glm::vec4(0, float(deep + 1) / float(AIR_MAX_DEEP + 1), float(deep + 1) / float(AIR_MAX_DEEP + 1), 1));
        if(is_devided){
            int ppos = get_pool_position(pool_position);
            for(int i = 0; i < 8; ++i)
                (nodes + ppos + i)->draw_debug(debug);
        }
    }

    std::vector<Quadable*> get_colliders(Quadable* quad) const {
        std::vector<Quadable*> out;
        _collect_intersections(quad->get_bounds(), out);
        return out;
    }

    void get_colliders(Quadable* quad,  std::vector<Quadable*>& out) const {
        _collect_intersections(quad->get_bounds(), out);
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

    std::vector<Quadable*> get(Quadable* quad) {
        return m_nodes[0].get_colliders(quad);
    }

    void get(Quadable* quad, std::vector<Quadable*>& out) const {
        m_nodes[0].get_colliders(quad, out);
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