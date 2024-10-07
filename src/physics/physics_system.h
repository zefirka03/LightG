#pragma once
#include <vector>
#include <functional>
#include <bitset>

#include "../core/core.h"
#include "physics_core.h"

class PhysicsBody : public Component {
private:
friend class PhysicsSystem;
    std::vector<std::function<void(PhysicsBody&, PhysicsBody&, collisionData const&)>> m_on_collide_handers;
    Collider* m_collider = nullptr;
public:
    int tag = 0;
    float m = 1.f;
    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    bool solid = false;

    PhysicsBody() {}
    ~PhysicsBody() { delete m_collider; }

    Collider* get_collider() const {
        return m_collider;
    }

    template<class Collider_t, class ...Args>
    Collider_t* set_collider(Args&&... args) {
        static_assert(std::is_base_of<Collider, Collider_t>::value, "Collider_t class must be derived by Collider");
        m_collider = new Collider_t(args...);
        m_collider->m_pb_handler = this;
        return static_cast<Collider_t*>(m_collider);
    }

    template<class T>
    void on_collide_add(T* object, void(T::*func)(PhysicsBody&, PhysicsBody&, collisionData const&)) {
        m_on_collide_handers.push_back(std::bind(func, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }
};

class PhysicsSystem : public System {
public:
    PhysicsSystem() {
        m_quadtree.resize(16);
        for (int i = 0; i < m_quadtree.size(); ++i)
            m_quadtree[i] = new Quadtree();
        m_tags_check.resize(16, 0xFFFF);
    }

    void set_tags(int tag_a, int tag_b, bool checkable) {
        m_tags_check[tag_a].set(tag_b, checkable);
        m_tags_check[tag_b].set(tag_a, checkable);
    }

    void draw_debug(DebugSystem& debug_system) {
        auto view_pb = m_registry->view<PhysicsBody>();
        view_pb.each([&](PhysicsBody& pb) {
            if (pb.m_collider) {
                pb.m_collider->draw_debug(debug_system);
                auto bbox = pb.m_collider->get_bounds();
                debug_system.draw_box(bbox.a, bbox.b, glm::vec4(1, 1, 0, 1));
            }
        });

        for(auto quadtree : m_quadtree)
            quadtree->draw_debug(debug_system);
    }
private:
    std::vector<Quadtree*> m_quadtree;
    std::vector<std::bitset<16>> m_tags_check;

    void init() override {}

    void start() override {

    }
    
    void update(float delta_time) override {
        // Get view of PB
        auto view_pb = m_registry->view<PhysicsBody, Transform>();
        //
         
        // Update transforms
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            transform.position += pb.velocity * delta_time;
            pb.velocity += pb.acceleration * delta_time;
        });
        //

        // Recreate quadtree
        for(auto quadtree : m_quadtree)
            quadtree->clear();
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            if (pb.m_collider) {
                pb.m_collider->update_transform(transform);
                m_quadtree[pb.tag]->add_child(pb.m_collider);
            }
        });
        for(auto quadtree : m_quadtree)
            quadtree->devide();
        //

        // Check collisions in quadtree
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            if (pb.solid) {
                // [!] Better to reserve
                std::vector<Quadable*> intersect_quads;
                for (int i = 0; i < m_quadtree.size(); ++i) 
                    if (m_tags_check[i].test(pb.tag))
                        m_quadtree[i]->get(pb.m_collider, intersect_quads);
                

                for (auto& quad : intersect_quads) {
                    Collider* collider_child = static_cast<Collider*>(quad);
                    if (collider_child != pb.m_collider) {
                        collisionData collision_data = pb.m_collider->check_collision(collider_child);
                        if (collision_data.is_collide) {
                            // Start custom handlers
                            for (auto on_collide : pb.m_on_collide_handers)
                                on_collide(*pb.m_collider->m_pb_handler, *collider_child->m_pb_handler, collision_data);

                            // Solve collisions
                            transform.position += collision_data.intersection_vector;
                            pb.velocity = -pb.velocity * 0.4f;
                        }
                    }
                }
            }
        });
        //
    }
};