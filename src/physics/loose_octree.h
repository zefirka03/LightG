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

    glm::vec3 center;
};

class LQuadTree {
private:
    struct LQuadableList {
        LQuadable* quad = nullptr;
        int next_child = -1;
    };

    std::vector<LQuadNode*> m_nodes;
    std::vector<LQuadableList> m_childs;

    boundingBox m_bounds;
public:
    LQuadTree() {}

    void insert(LQuadable& child) {
        m_childs.emplace_back(&child);
        m_bounds.adjust_bounds(child.get_bounds());
    }

    void devide() {
        

        for(auto& child : m_childs) {

        }
    }
};