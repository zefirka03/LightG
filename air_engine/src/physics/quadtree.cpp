#include "quadtree.h"

inline int Quadtree::_get_pool_position(int i) const {
    return 8 * i + 1;
}

inline int Quadtree::_get_pool_position_up(int i) const {
    return (i - 1) / 8;
}

void Quadtree::_add_child(int node_it, int child_it) {
    auto& node = m_nodes[node_it];
    if (node.child_last == -1) {
        node.child_first = child_it;
        node.child_last = child_it;
    } else {
        m_childs[node.child_last].child_next = child_it;
        m_childs[child_it].child_prev = node.child_last;
        node.child_last = child_it;
    }

    m_childs[child_it].node_it = node_it;
    ++node.childs_size;
    node.bounds.adjust_bounds(m_childs[child_it].quad->get_bounds());
}

void Quadtree::_pop_child(int child_it) {
    auto& childs = m_childs[child_it];
    if (childs.child_prev != -1) 
        m_childs[childs.child_prev].child_next = childs.child_next;
    if (childs.child_next != -1) 
        m_childs[childs.child_next].child_prev = childs.child_prev;
    if (childs.node_it != -1) {
        --m_nodes[childs.node_it].childs_size;
        if (m_nodes[childs.node_it].child_first == child_it)
            m_nodes[childs.node_it].child_first = m_childs[m_nodes[childs.node_it].child_first].child_next;
        if (m_nodes[childs.node_it].child_last == child_it)
            m_nodes[childs.node_it].child_last = m_childs[m_nodes[childs.node_it].child_last].child_prev;
    }
    childs.child_next = -1;
    childs.child_prev = -1;
    childs.node_it = -1;
}

void Quadtree::_devide(int node_it, bool equad_sizes) {
    auto& node = m_nodes[node_it];
    if (node.deep < AIR_MAX_DEEP && node.childs_size >= AIR_QUAD_DEVIDE_SIZE) {
        int ppos = _get_pool_position(node.pool_position);
        for (int i = 0; i < 8; ++i)
            m_nodes[ppos + i] = QuadNode(node.deep + 1, ppos + i);
        node.is_devided = true;
        int it = 0;
        for (int ch = node.child_first, next_ch = -1; ch != -1; ch = next_ch) {
            int quadrant = _get_quadrant(node_it, m_childs[ch].quad->get_bounds());
            next_ch = m_childs[ch].child_next;
            if(quadrant != -1) {
                _pop_child(ch);
                _add_child(ppos + quadrant, ch);
            }
        }
        for (int i = 0; i < 8; ++i) {
            if (equad_sizes) 
                _fill_bounds(ppos + i);
            _devide(ppos + i, equad_sizes);
        }
    }
}

int Quadtree::_get_quadrant(int node_it, boundingBox const& box) {
    glm::vec3 const& a = box.get_a();
    glm::vec3 const& b = box.get_b();
    glm::vec3 const& middle = m_nodes[node_it].bounds.get_center();
    if (a.x > middle.x) {
        if (a.y > middle.y) {
            if (a.z > middle.z) {
                return 0;
            }
            else if (b.z <= middle.z) {
                return 1;
            }
        }
        else if (b.y <= middle.y) {
            if (a.z > middle.z) {
                return 2;
            }
            else if (b.z <= middle.z) {
                return 3;
            }
        }
    }
    else if (b.x <= middle.x) {
        if (a.y > middle.y) {
            if (a.z > middle.z) {
                return 4;
            }
            else if (b.z <= middle.z) {
                return 5;
            }
        }
        else if (b.y <= middle.y) {
            if (a.z > middle.z) {
                return 6;
            }
            else if (b.z <= middle.z) {
                return 7;
            }
        }
    }
    return -1;
}

void Quadtree::_ray_traversal(int node_it, Ray const& ray, std::vector<Quadable*>& out) {
    auto& node = m_nodes[node_it];
    float t;
    if (ray.intersect(node.bounds, t)) {
        for (int i = node.child_first; i != -1; i = m_childs[i].child_next)
            out.emplace_back(m_childs[i].quad);
        Ray new_ray(
            ray.origin + (t + 0.001f) * ray.direction,
            ray.direction,
            ray.length - t
        );
        if (new_ray.length < 0) return;
        if (node.is_devided) {
            int ppos = _get_pool_position(node.pool_position);
            for (int i = 0; i < 8; ++i)
                _ray_traversal(ppos + i, new_ray, out);
        }
    }
}

void Quadtree::_get_all_colliders(int node_it, std::vector<Quadable*>& out) const {
    auto& node = m_nodes[node_it];
    for (int i = node.child_first; i != -1; i = m_childs[i].child_next)
        out.emplace_back(m_childs[i].quad);
    // other
    if (node.is_devided) {
        int ppos = _get_pool_position(node.pool_position);
        for (int i = 0; i < 8; ++i)
            _get_all_colliders(ppos + i, out);
    }
}

void Quadtree::_collect_intersections(int node_it, boundingBox const& box, std::vector<Quadable*>& array) const {
    auto& node = m_nodes[node_it];
    // this quadnode
    if (box.intersect(node.bounds)) {
        for (int i = node.child_first; i != -1; i = m_childs[i].child_next)
            array.emplace_back(m_childs[i].quad);
        // other
        if (node.is_devided) {
            int ppos = _get_pool_position(node.pool_position);
            for (int i = 0; i < 8; ++i)
                _collect_intersections(ppos + i, box, array);
        }
    }
}

std::vector<Quadable*> Quadtree::_get_colliders(int node_it, Quadable* quad) const {
    std::vector<Quadable*> out;
    _collect_intersections(node_it, quad->get_bounds(), out);
    return out;
}

void Quadtree::_get_colliders(int node_it, Quadable* quad, std::vector<Quadable*>& out) const {
    _collect_intersections(node_it, quad->get_bounds(), out);
}

void Quadtree::_draw_debug(int node_it, DebugSystem& debug, glm::vec4 color) {
    auto& node = m_nodes[node_it];
    debug.draw_box(node.bounds.get_a(), node.bounds.get_b(), color * (float(node.deep + 1) / float(AIR_MAX_DEEP + 1)));

    if (node.is_devided) {
        int ppos = _get_pool_position(node.pool_position);
        for (int i = 0; i < 8; ++i)
            _draw_debug(ppos + i, debug, color);
    }
}
 
void Quadtree::_fill_bounds(int node_it) {
    if (node_it <= 0 || node_it >= m_pool_size)
        return;

    auto& curr_node = m_nodes[node_it];
    auto& upper_node = m_nodes[_get_pool_position_up(node_it)];
    const glm::vec3& middle = upper_node.bounds.get_center();
    const glm::vec3 half = (upper_node.bounds.get_b() - upper_node.bounds.get_a()) / 2.f;

    switch((node_it - 1) % 8) {
        case 0:
            curr_node.bounds = boundingBox(
                middle,
                upper_node.bounds.get_b()
            );
            break;
        case 1:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(0, 0, half.z),
                upper_node.bounds.get_b() - glm::vec3(0, 0, half.z)
            );
            break;
        case 2:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(0, half.y, 0),
                upper_node.bounds.get_b() - glm::vec3(0, half.y, 0)
            );
            break;
        case 3:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(0, half.y, half.z),
                upper_node.bounds.get_b() - glm::vec3(0, half.y, half.z)
            );
            break;
        case 4:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(half.x, 0, 0),
                upper_node.bounds.get_b() - glm::vec3(half.x, 0, 0)
            );
            break;
        case 5:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(half.x, 0, half.z),
                upper_node.bounds.get_b() - glm::vec3(half.x, 0, half.z)
            );
            break;
        case 6:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(half.x, half.y, 0),
                upper_node.bounds.get_b() - glm::vec3(half.x, half.y, 0)
            );
            break;
        case 7:
            curr_node.bounds = boundingBox(
                middle - glm::vec3(half.x, half.y, half.z),
                upper_node.bounds.get_b() - glm::vec3(half.x, half.y, half.z)
            );
            break;
        default:
            break;
    }
}

Quadtree::Quadtree() {
    m_pool_size = (2 << (3 * (AIR_MAX_DEEP + 1)) - 1) / 7;
    m_nodes = new QuadNode[m_pool_size]();
    m_nodes[0].deep = 0;
    m_nodes[0].pool_position = 0;
    m_childs_size = 0;
}

void Quadtree::add_child(Quadable* child) {
    m_childs.emplace_back(child);
    _add_child(0, m_childs_size);
    ++m_childs_size;
}

void Quadtree::draw_debug(DebugSystem& debug, glm::vec4 color){
    _draw_debug(0, debug, color);
}

void Quadtree::devide(bool equal_sizes) {
    _devide(0, equal_sizes);
}

std::vector<Quadable*> Quadtree::get(Quadable* quad) {
    return _get_colliders(0, quad);
}

void Quadtree::get(Quadable* quad, std::vector<Quadable*>& out) const {
    _get_colliders(0, quad, out);
}

void Quadtree::get_all(std::vector<Quadable*>& out) const {
    _get_all_colliders(0, out);
}

void Quadtree::ray_traversal(Ray const& ray, std::vector<Quadable*>& out){
    float t;
    if (!ray.intersect(m_nodes[0].bounds, t))
        return;
    Ray new_ray(
        ray.origin + (t + 0.001f) * ray.direction,
        ray.direction,
        ray.length - t - 0.001f
    );
    
    _ray_traversal(0, new_ray, out);
}

QuadNode const* Quadtree::get_quadnodes_pool() const {
    return m_nodes;
}

int Quadtree::get_quadnodes_pool_size() const {
    return m_pool_size;
}

std::vector<QuadableList> const& Quadtree::get_quadlist() const {
    return m_childs;
}

void Quadtree::fill_bounds() {
    for (int i = 0; i < m_pool_size; ++i)
        _fill_bounds(i);
}

void Quadtree::clear() {
    for (int i = 0; i < m_pool_size; ++i)
        m_nodes[i].reset();
    m_childs.clear();
    m_childs_size = 0;
}

inline int Quadtree::size() const {
    return m_childs_size;
}

Quadtree::~Quadtree() {
    delete[] m_nodes;
}
