#pragma once
#include "../core/core.h"
#include "physics_core.h"

struct PhysicsBody : public Component {
    Collider* collider;

    float m;
    glm::vec3 acceleration;
    glm::vec3 velocity;
};

class PhysicsSystem : public System {
public:
    void draw_debug(DebugSystem& debug_system) {
        auto view_pb = m_registry->view<PhysicsBody>();
        view_pb.each([&](PhysicsBody& pb) {
            pb.collider->draw_debug(debug_system);
            auto bbox = pb.collider->get_bounds();
            debug_system.draw_box(bbox.a, bbox.b, glm::vec4(1, 1, 0, 1));
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
            pb.collider->update_transform(transform);
            m_quadtree->add_child(*pb.collider);
        });

        m_quadtree->devide();
    }
};