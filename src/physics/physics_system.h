#pragma once
#include "../core/core.h"
#include "physics_core.h"

struct PhysicsBehavior {
    virtual void on_collide(Collider& body) {};
};

struct PhysicsBody : public Component {
    Collider* collider = nullptr;
    PhysicsBehavior* behavior = nullptr;

    float m;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    PhysicsBody() {}
    PhysicsBody(PhysicsBehavior* behavior) : behavior(behavior) {}
    ~PhysicsBody() { delete behavior; delete collider; }
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
        m_quadtree->clear();

        auto view_pb = m_registry->view<PhysicsBody, Transform>();
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            if (pb.collider) {
                pb.collider->update_transform(transform);
                m_quadtree->add_child(pb.collider);
            }
        });

        m_quadtree->devide();

        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            if (pb.behavior) {
                auto& childs = m_quadtree->get(pb.collider);

                for (auto& child : childs)
                    pb.behavior->on_collide(dynamic_cast<Collider&>(*child));
            }
        });
    }
};