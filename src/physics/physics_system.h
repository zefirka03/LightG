#pragma once
#include <vector>
#include <functional>
#include "../core/core.h"
#include "physics_core.h"

struct PhysicsBehavior {
    virtual void on_collide(Collider& body) {};
};

struct PhysicsBody : public Component {
private:
friend class PhysicsSystem;
    std::vector<std::function<void(Collider&, Collider&)>> m_on_collide_handers;
public:
    Collider* collider = nullptr;

    float m = 1.f;
    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);

    PhysicsBody() {}
    ~PhysicsBody() { delete collider; }

    template<class T>
    void on_collide_add(T* object, void(T::*func)(Collider&, Collider&)) {
        m_on_collide_handers.push_back(std::bind(func, object, std::placeholders::_1, std::placeholders::_2));
    }
};

class PhysicsSystem : public System {
public:
    void draw_debug(DebugSystem& debug_system) {
        auto view_pb = m_registry->view<PhysicsBody>();
        view_pb.each([&](PhysicsBody& pb) {
            if (pb.collider) {
                pb.collider->draw_debug(debug_system);
                auto bbox = pb.collider->get_bounds();
                debug_system.draw_box(bbox.a, bbox.b, glm::vec4(1, 1, 0, 1));
            }
        });

        if(m_quadtree)
            m_quadtree->draw_debug(debug_system);
    }
private:
    Quadtree* m_quadtree = nullptr;

    void init() override {
        m_quadtree = new Quadtree();
    }

    void start() override {

    }
    
    void update(float delta_time) override {
        // Recreate quadtree
        m_quadtree->clear();
        auto view_pb = m_registry->view<PhysicsBody, Transform>();
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            if (pb.collider) {
                pb.collider->update_transform(transform);
                m_quadtree->add_child(pb.collider);
            }
        });
        m_quadtree->devide();

        // Check collisions in quadtree
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            for(auto on_collide : pb.m_on_collide_handers){
                auto& childs = m_quadtree->get(pb.collider);

                for (auto& child : childs) {
                    Collider* collider_child = static_cast<Collider*>(child);
                    if (collider_child != pb.collider) {
                        if (pb.collider->check_collision(collider_child))
                            on_collide(*pb.collider, *collider_child);
                    }
                }
            }
        });
    }
};