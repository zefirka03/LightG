// https://www.yumpu.com/en/document/read/33628186/loose-octree-a-data-structure-for-the-simulation-of-polydisperse-
#pragma once
#include <array>
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
    int deep_down = -1;

    int next_node = -1;

    glm::vec3 center;
    float radius;

    void reset(){
        first_child = -1;
        last_child = -1;
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

    std::array<glm::vec3, MAX_DEEP + 1> m_cached_side_sizes;
    std::vector<std::array<int, 8>> m_cached_down_indices;

    void _get_potential_colliders(LQuadable* quad) {

    }

    void _update_dinamic_cache() {
        glm::vec3 init_size = m_bounds.get_b() - m_bounds.get_a();
        for (int d = 0; d < MAX_DEEP + 1; ++d) {
            m_cached_side_sizes[d] = init_size;
            init_size /= 2.f;
        }
    }

    int _get_index(glm::vec3 const& center, int deep) {
        const int first_it = ((1 << (3 * deep)) - 1) / 7;
        const int sides = 1 << deep;
        glm::vec3 pos = (center - m_bounds.get_a()) / m_cached_side_sizes[deep];
        return first_it + int(pos.x) + int(pos.z) * sides + int(pos.y) * sides * sides;
    }

    void _cache_down_indices() {
        int nodes_count = ((1 << (3 * (MAX_DEEP + 1))) - 1) / 7;
        m_cached_down_indices.resize(nodes_count);
        int side_size = 2;
        int half_side_size = 1;
        int last_pos = 0;
        int curr_pos = 1;

        for (int d = 1; d < MAX_DEEP + 1; ++d) {
            for (int x = 0; x < side_size; ++x) {
                for (int y = 0; y < side_size; ++y) {
                    for (int z = 0; z < side_size; ++z) {
                        int parent_index = last_pos + (x / 2) + (y / 2) * half_side_size * half_side_size + (z / 2) * half_side_size;

                        m_cached_down_indices[parent_index][x % 2 + y % 2 * 2 * 2 + z % 2 * 2] = curr_pos;
                        ++curr_pos;
                    }
                }
            }
            last_pos += half_side_size * half_side_size * half_side_size;
            half_side_size = side_size;
            side_size <<= 1;
        }
    }

    std::array<int, 8>& _get_down_indexes(int index) {
        return m_cached_down_indices[index];
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

    void _setup_nodes(int node_it, glm::vec3 const& center, float outer_radius, int deep) {
        auto& node = m_nodes[node_it];
        node.center = center;
        node.deep = deep;
        node.radius = outer_radius;
        if (deep >= m_deep) return;

        glm::vec3 displace = glm::vec3(1.f / std::sqrt(3.f)) * outer_radius / 4.f;
        const float half_outer_radius = outer_radius / 2.f;
        
        int last = -1;
        int* index = &_get_down_indexes(node_it)[0];

        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                for (int x = 0; x < 2; ++x) {
                    glm::vec3 curr_center = center + displace;
                    const int indx = *index;

                    if (last != -1) {
                        m_nodes[last].next_node = indx;
                        last = indx;
                    } else {
                        node.deep_down = indx;
                        last = indx;
                    }
                    _setup_nodes(
                        indx,
                        curr_center,
                        half_outer_radius,
                        deep + 1
                    );

                    ++index;
                    displace.z = -displace.z;
                }
                displace.y = -displace.y;
            }
            displace.x = -displace.x;
        }
    }

public:
    LOctree() {
        _cache_down_indices();
    }

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

        _update_dinamic_cache();
        _setup_nodes(0, m_bounds.get_center(), m_bounds.get_diameter(), 0);

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