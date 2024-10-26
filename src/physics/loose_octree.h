// https://www.yumpu.com/en/document/read/33628186/loose-octree-a-data-structure-for-the-simulation-of-polydisperse-
#pragma once
#include <array>
#include <vector>
#include <sstream>
#include "bounding_box.h"

#define MAX_DEEP 4

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
    int childs_count = 0;

    int deep = -1;
    int deep_down = -1;

    int next_node = -1;

    glm::vec3 center;
    float radius;

    void reset(){
        first_child = -1;  
        last_child = -1;
        childs_count = 0;
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
    boundingBox m_last_bounds;

    float m_min_diameter = FLT_MAX;
    int m_deep = 0;

    std::array<glm::vec3, MAX_DEEP + 1> m_cached_side_sizes;
    std::vector<std::array<int, 8>> m_cached_down_indices;

    void _expand_bounds(float t) {
        m_bounds.set(
            m_last_bounds.get_center() + t * (m_last_bounds.get_a() - m_last_bounds.get_center()),
            m_last_bounds.get_center() + t * (m_last_bounds.get_b() - m_last_bounds.get_center())
        );

        printf("----------Expanded----------\n");

        //printf("-----------------\n");
        //printf("bounds a: %f %f %f\n", m_bounds.get_a().x, m_bounds.get_a().y, m_bounds.get_a().z);
        //printf("bounds b: %f %f %f\n", m_bounds.get_b().x, m_bounds.get_b().y, m_bounds.get_b().z);
        //printf("deep: %d\n", deep);
        //printf("s: %f\n", s);
        //printf("min diameter: %f\n", m_min_diameter);
        //printf("nodes size: %d\n", m_nodes.size());
        //printf("childs size: %d\n", m_childs.size());
        //printf("-----------------\n");
    }

    bool _is_spheres_intersect(glm::vec3 c1, float r1, glm::vec3 c2, float r2) const {
        return glm::length(c2 - c1) < (r1 + r2);
    }

    bool _is_quad_node_intersect(LQuadable* quad, LQuadNode const& node) const {
        return _is_spheres_intersect(
            quad->get_bounds().get_center(), quad->get_bounds().get_diameter() / 2,
            node.center, node.radius
        );
    }

    void _get_potential_colliders(LQuadable* quad, int node_it, std::vector<LQuadable*>& colliders) const {
        auto& node = m_nodes[node_it];
        if (_is_quad_node_intersect(quad, node)) {
            // get this node colliders
            for (
                int c = node.first_child;
                c != -1;
                c = m_childs[c].next_child
            ) {
                colliders.emplace_back(m_childs[c].quad);
            }

            // recursive try to get subnodes colliders
            for (
                int n = node.deep_down;
                n != -1;
                n = m_nodes[n].next_node
            ) {
                _get_potential_colliders(quad, n, colliders);
            }
        }
    }

    void _update_dinamic_cache() {
        glm::vec3 init_size = (m_bounds.get_b() - m_bounds.get_a()) * (1.f + 0.001f);
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
            for (int y = 0; y < side_size; ++y) {
                for (int z = 0; z < side_size; ++z) {
                    for (int x = 0; x < side_size; ++x) {
                        const int parent_index = last_pos + (x / 2) + (y / 2) * half_side_size * half_side_size + (z / 2) * half_side_size;

                        m_cached_down_indices[parent_index][(x % 2) + (y % 2) * 2 * 2 + (z % 2) * 2] = curr_pos;
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
        ++node.childs_count;
    }

    void _setup_nodes(int node_it, glm::vec3 const& center, float outer_radius, int deep) {
        auto& node = m_nodes[node_it];
        node.center = center;
        node.deep = deep;
        node.radius = outer_radius;
        if (deep >= m_deep) return;

        glm::vec3 displace = -glm::vec3(1.f / std::sqrt(3.f)) * outer_radius / 4.f;
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
                    displace.x = -displace.x;
                }
                displace.z = -displace.z;
            }
            displace.y = -displace.y;
        }
    }

    void _nodes_check_state() {

        if (!m_childs.empty() && !m_bounds.contains(m_last_bounds)) {
            // Make m_bounds cubic
            glm::vec3 sizes = m_last_bounds.get_b() - m_last_bounds.get_a();
            float max_size = std::max(std::max(sizes.x, sizes.y), sizes.z);
            m_last_bounds.set_b(m_last_bounds.get_a() + glm::vec3(max_size));

            _expand_bounds(2.f);

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
        }
    }

    void _print_debug(int node_it) {
        if (m_nodes[node_it].childs_count) {
            for (int i = 0; i < m_nodes[node_it].deep; ++i)
                printf("-");
            printf("node: %d, radius: %f, childs: %d\n", node_it, m_nodes[node_it].radius, m_nodes[node_it].childs_count);
            
            for (
                int c = m_nodes[node_it].first_child;
                c != -1;
                c = m_childs[c].next_child
            ) {
                for (int i = 0; i < m_nodes[node_it].deep + 1; ++i)
                    printf("*");
                printf("child: %d, radius: %f\n", c, m_childs[c].quad->get_bounds().get_diameter()/2.f);
            }
        }

        for (
            int n = m_nodes[node_it].deep_down;
            n != -1;
            n = m_nodes[n].next_node
        ) {
            _print_debug(n);
        }
    }

public:
    LOctree() {
        _cache_down_indices();
    }

    void insert(LQuadable* child) {
        m_childs.emplace_back(child);
        m_last_bounds.adjust_bounds(child->get_bounds());

        m_min_diameter = std::min(m_min_diameter, child->get_bounds().get_diameter());
    }

    void devide() {
        _nodes_check_state();

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

    void get(LQuadable* quad, std::vector<LQuadable*>& colliders) {
        _get_potential_colliders(quad, 0, colliders);
    }

    void print_debug() {
        printf("------START------\n");
        _print_debug(0);
        printf("-------END-------\n");
    }

    void clear() {
        m_childs.clear();
        for (auto& node : m_nodes)
            node.reset();
        //m_nodes.clear();
        m_last_bounds = boundingBox();

        m_min_diameter = FLT_MAX;
    }
};