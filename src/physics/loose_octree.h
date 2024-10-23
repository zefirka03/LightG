// https://www.yumpu.com/en/document/read/33628186/loose-octree-a-data-structure-for-the-simulation-of-polydisperse-
#pragma once
#include <vector>
#include "bounding_box.h"

#define MAX_DEEP 5

class LQuadable {
friend class LQuadNode;
protected:
    boundingBox m_bbox;
    virtual void update_bounds() = 0;
public:
    inline boundingBox const& get_bounds() const {
        return m_bbox;
    };
};

struct LQuadNode {
    int first_child = -1;
    int last_child = -1;

    int deep = -1;
    int deep_up = -1;
    int deep_down = -1;

    int next_node = -1;

    glm::vec3 center;
    float side_size;

    void reset(){
        first_child = -1;
        last_child = -1;
        deep_up = -1;
        deep_down = -1;
    }
};

class LOctree {
private:
    struct LQuadableList {
        LQuadable* quad = nullptr;
        int next_child = -1;
    };

    // 0 index always root
    std::vector<LQuadNode> m_nodes;
    std::vector<LQuadableList> m_childs;

    boundingBox m_bounds;

    float m_min_diameter = FLT_MAX;
    int m_deep = 0;

    void _find_and_place(int quad_it) {
        auto& quad_l = m_childs[quad_it];
        auto& quad = quad_l.quad;
        int deep_pos = std::min(int(std::log(m_bounds.get_diameter() / quad->get_bounds().get_diameter())), MAX_DEEP);
        int first_it = ((1 << (3 * deep_pos)) - 1) / 7;

        glm::vec3 pos = (quad->get_bounds().get_center() - m_bounds.get_a()) / m_nodes[first_it].side_size;
        auto& node = m_nodes[first_it + int(pos.x) * 4 + int(pos.y) * 2 + pos.z];

        if (node.first_child == -1) {
            node.first_child = quad_it;
            node.last_child = quad_it;
        }
        else {
            m_childs[node.last_child].next_child = quad_it;
            node.last_child = quad_it;
        }
    }

    void _get_potential_colliders(LQuadable* quad) {

    }

    int _get_index(glm::vec3 const& center, int deep) {
        const int first_it = ((1 << (3 * deep)) - 1) / 7;
        const int sides = 1 << deep;
        const glm::vec3 side_size = (m_bounds.get_b() - m_bounds.get_a()) / float(sides);
        glm::vec3 pos = (center - m_bounds.get_a()) / side_size;
        return first_it + int(pos.x) + int(pos.y) * sides + int(pos.z) * sides * sides;
    }

    int _get_deep(float diameter) {
        const float rate = m_bounds.get_diameter() / diameter;
        return std::min(rate > 1 ? int(std::log(rate)) : 0, m_deep);
    }

    void _add_child(int node_it, int child_it) {
        auto& node = m_nodes[node_it];
        if (node.first_child != -1) {
            m_childs[node.last_child].next_child = child_it;
            node.last_child = child_it;
        }
        else {
            node.first_child = child_it;
            node.last_child = child_it;
        }
    }

public:
    LOctree() {}

    void insert(LQuadable* child) {
        m_childs.emplace_back(child);
        m_bounds.adjust_bounds(child->get_bounds());

        m_min_diameter = std::min(m_min_diameter, child->get_bounds().get_diameter());
    }

    void devide() {
        // Make m_bounds cubic
        {
            glm::vec3 sizes = m_bounds.get_b() - m_bounds.get_a();
            float max_size = std::max(std::max(sizes.x, sizes.y), sizes.z);
            m_bounds.set_b(m_bounds.get_a() + glm::vec3(max_size));
        }

        // Calculate new deep, nodes_count...
        float s = m_bounds.get_diameter() / std::sqrt(3.f);
        m_deep = std::min(int(std::log2(std::max(s / m_min_diameter, 1.f))) + 1, MAX_DEEP);
        int nodes_count = ((1 << (3 * (m_deep + 1))) - 1) / 7;

        m_nodes.resize(nodes_count);

        //printf("-----------------\n");
        //printf("bounds a: %f %f %f\n", m_bounds.get_a().x, m_bounds.get_a().y, m_bounds.get_a().z);
        //printf("bounds a: %f %f %f\n", m_bounds.get_b().x, m_bounds.get_b().y, m_bounds.get_b().z);
        //printf("deep: %d\n", deep);
        //printf("s: %f\n", s);
        //printf("min diameter: %f\n", m_min_diameter);
        //printf("nodes size: %d\n", m_nodes.size());
        //printf("childs size: %d\n", m_childs.size());
        //printf("-----------------\n");

        // Add childs
        for (int child_i = 0; child_i < m_childs.size(); ++child_i) {
            _add_child(
                _get_index(
                    m_childs[child_i].quad->get_bounds().get_center(),
                    _get_deep(m_childs[child_i].quad->get_bounds().get_diameter())
                ),
                child_i
            );
        }
    }

    void clear() {
        m_childs.clear();
        m_nodes.clear();

        m_min_diameter = FLT_MAX;
    }
};