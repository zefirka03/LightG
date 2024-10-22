// https://www.yumpu.com/en/document/read/33628186/loose-octree-a-data-structure-for-the-simulation-of-polydisperse-
#pragma once
#include <vector>
#include "bounding_box.h"

class LQuadable {
friend class LQuadNode;
private:
    boundingBox m_bbox;
protected:
    virtual void update_bounds() = 0;
public:
    inline boundingBox const& get_bounds() const {
        return m_bbox;
    };
};

struct LQuadNode {
    int first_child = -1;
    int last_child = -1;
    int deep_it = -1;

    boundingBox bounds;

    void reset(){
        first_child = -1;
        last_child = -1;
        deep_it = -1;
    }
};

class LQuadTree {
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

    void _find_and_place(LQuadable* quad){
        
    }
public:
    LQuadTree() {}

    void insert(LQuadable& child) {
        m_childs.emplace_back(&child);
        m_bounds.adjust_bounds(child.get_bounds());

        m_min_diameter = std::min(m_min_diameter, child.get_bounds().get_diameter());
    }

    void devide() {
        // Make m_bounds cubic
        {
            glm::vec3 sizes = m_bounds.get_b() - m_bounds.get_a();
            float max_size = std::max(std::max(sizes.x, sizes.y), sizes.z);
            m_bounds.set_b(m_bounds.get_a() + glm::vec3(max_size));
        }

        // Calculate new deep, nodes_count...
        float s = m_bounds.get_diameter() / std::sqrt(2.f);
        int deep = int(std::log2(s / m_min_diameter)) + 1;
        int nodes_count = 1 << deep;

        m_nodes.resize(nodes_count);

        // Init root node
        m_nodes[0].bounds = m_bounds;

        // Childs
        for(auto& child : m_childs) {

        }
    }
};