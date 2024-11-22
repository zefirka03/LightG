#pragma once
#include <vector>
#include "../debug/debug.h"
#include "bounding_box.h"
#include "ray.h"

#define AIR_QUAD_DEVIDE_SIZE 16
#define AIR_MAX_DEEP 3

struct Quadable {
protected:
    boundingBox m_bbox;
public:
    virtual void update_bounds() = 0;

    boundingBox const& get_bounds() const {
        return m_bbox;
    };
};

/// 
/// First you add all childs to root node, then do devide
/// 
struct QuadNode {
    boundingBox bounds = boundingBox();
    int childs_size = 0;
    int child_first = -1;
    int child_last = -1;
    int deep = 0;
    int pool_position = 0;
    bool is_devided = false;

    QuadNode();
    QuadNode(int _deep, int _pool_position);
    void reset();
};

struct QuadableList {
    int child_next = -1;
    int child_prev = -1;
    int node_it = -1;
    Quadable* quad = nullptr;

    QuadableList() {}
    QuadableList(Quadable* _quad) : quad(_quad) {}
};

class Quadtree {
private:
    QuadNode* m_nodes;
    std::vector<QuadableList> m_childs;
    int m_pool_size;
    int m_childs_size;

    inline int _get_pool_position(int i) const;
    inline int _get_pool_position_up(int i) const;
    void _add_child(int node_it, int child_it);
    void _pop_child(int child_it);
    void _devide(int node_it, bool equad_sizes = false);
    int _get_quadrant(int node_it, boundingBox const& box);
    void _ray_traversal(int node_it, Ray const& ray, std::vector<Quadable*>& out);
    void _get_all_colliders(int node_it, std::vector<Quadable*>& out) const;
    void _collect_intersections(int node_it, boundingBox const& box, std::vector<Quadable*>& array) const;
    std::vector<Quadable*> _get_colliders(int node_it, Quadable* quad) const;
    void _get_colliders(int node_it, Quadable* quad, std::vector<Quadable*>& out) const;
    void _draw_debug(int node_it, DebugSystem& debug, glm::vec4 color = glm::vec4(0, 1, 1, 1));
    void _fill_bounds(int node_it);

public:
    Quadtree();
    ~Quadtree();

    void add_child(Quadable* child);
    void devide(bool equal_sizes = false);
    std::vector<Quadable*> get(Quadable* quad);
    void ray_traversal(Ray const& ray, std::vector<Quadable*>& out);

    void get(Quadable* quad, std::vector<Quadable*>& out) const;
    void get_all(std::vector<Quadable*>& out) const;
    QuadNode const* get_quadnodes_pool() const;
    int get_quadnodes_pool_size() const;
    std::vector<QuadableList> const& get_quadlist() const;
    inline int size() const;

    void fill_bounds();
    void clear();

    void draw_debug(DebugSystem& debug, glm::vec4 color = glm::vec4(0, 1, 1, 1));
};