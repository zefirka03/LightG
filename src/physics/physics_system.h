#pragma once
#include <vector>
#include <functional>
#include <bitset>

#include "../core/core.h"
#include "physics_core.h"

#define AIR_PHYSICS_MAX_TAGS 16

class PhysicsBody : public Component {
private:
friend class PhysicsSystem;
    std::vector<std::function<void(PhysicsBody&, PhysicsBody&, collisionData const&)>> m_on_collide_handlers;
    Collider* m_collider = nullptr;

    glm::vec3 m_last_force = glm::vec3(0);
public:
    enum pbType : int {
        SOLID,
        RIGID,
    };

    int tag = 0;
    int type = pbType::SOLID;

    float m = 1.f;
    float bouncyness = 1.f;
    float friction = 0.f;

    glm::vec3 force = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);

    PhysicsBody() {}
    ~PhysicsBody() { delete m_collider; }

    PhysicsBody(PhysicsBody&& pb) noexcept {
        *this = std::move(pb);
    }

    PhysicsBody& operator=(PhysicsBody&& pb) noexcept {
        entity = pb.entity;
        scene = pb.scene;

        delete m_collider; 
        m_collider = pb.m_collider;
        pb.m_collider = nullptr;

        if (m_collider) {
            m_collider->m_pb_handler = this;
            m_collider->m_transform_handler = &scene->get_component<Transform>(entity);
        }

        m_last_force = pb.m_last_force;
        m_on_collide_handlers = pb.m_on_collide_handlers;

        tag = pb.tag;
        type = pb.type;

        m = pb.m;
        bouncyness = pb.bouncyness;
        friction = pb.friction;

        force = pb.force;
        velocity = pb.velocity;
        return *this;
    }

    PhysicsBody(PhysicsBody const&) = delete;
    PhysicsBody& operator=(PhysicsBody const&) = delete;

    Collider* get_collider() const {
        return m_collider;
    }

    template<class Collider_t, class ...Args>
    Collider_t* set_collider(Args&&... args) {
        static_assert(std::is_base_of<Collider, Collider_t>::value, "Collider_t class must be derived by Collider");
        m_collider = new Collider_t(args...);
        m_collider->m_pb_handler = this;
        m_collider->m_transform_handler = &this->scene->get_component<Transform>(this->entity);
        m_collider->m_cached_type = check_collider_type<Collider_t>();
        return static_cast<Collider_t*>(m_collider);
    }

    template<class T>
    void on_collide_add(T* object, void(T::*func)(PhysicsBody&, PhysicsBody&, collisionData const&)) {
        m_on_collide_handlers.push_back(std::bind(func, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void apply_force(glm::vec3 const& _force) {
        force += _force;
    }
};

class PhysicsSystem : public System {
public:
    PhysicsSystem() {
        m_quadtree.resize(AIR_PHYSICS_MAX_TAGS);
        for (int i = 0; i < m_quadtree.size(); ++i) {
            m_quadtree[i] = {
                .tag = i,
                .quadtree = new Quadtree(),
                .intersections = std::vector<Quadable*>()
            };
        }

        m_tags_check.resize(AIR_PHYSICS_MAX_TAGS);
        for (int i = 0; i < m_tags_check.size(); ++i)
            m_tags_check[i].set();
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
                debug_system.draw_box(bbox.get_a(), bbox.get_b(), glm::vec4(1, 1, 0, 1));
            }
        });

        for(auto quadtree : m_quadtree)
            quadtree.quadtree->draw_debug(debug_system);
    }

    void ray_intersection(Ray const& ray, std::vector<Collider>& out) {
        std::vector<Quadable*> potential_quads;
        for (int i = 0; i < m_quadtree.size(); ++i) 
            m_quadtree[i].quadtree->ray_traversal(ray, potential_quads);
        
        printf("Potential quads: %d \n", potential_quads.size());
    }

private:
    struct QtIntTag {
        int tag;
        Quadtree* quadtree;
        std::vector<Quadable*> intersections;
    };
    std::vector<QtIntTag> m_quadtree;
    std::vector<std::bitset<AIR_PHYSICS_MAX_TAGS>> m_tags_check;

    void init() override {}

    void start() override {

    }
    
    void update(float delta_time) override {
        // Get view of PB
        auto view_pb = m_registry->view<PhysicsBody, Transform>();
        //
         
        // Update transforms
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            pb.m_collider->m_pb_handler = &pb;
            pb.m_collider->m_transform_handler = &transform;
            pb.m_collider->m_cached_already_resolved = false;

            transform.position += pb.velocity * delta_time;
            pb.velocity += (pb.force / pb.m) * delta_time;
            pb.m_last_force = pb.force;
            pb.force = glm::vec3(0);
            pb.m_collider->update_bounds();
        });
        //

        // Recreate quadtrees
        for (int i = 0; i < m_quadtree.size(); ++i) {
            m_quadtree[i].tag = i;
            m_quadtree[i].quadtree->clear();
            m_quadtree[i].intersections.clear();
        }
        view_pb.each([&](PhysicsBody& pb, Transform& transform) {
            if (pb.m_collider) {
                m_quadtree[pb.tag].quadtree->add_child(pb.m_collider);
                m_quadtree[pb.tag].intersections.emplace_back(pb.m_collider);
            }
        });
        for (int i = 0; i < m_quadtree.size(); ++i)
            m_quadtree[i].quadtree->devide();
        //

        // Get all quads
        std::sort(m_quadtree.begin(), m_quadtree.end(), [](QtIntTag const& a, QtIntTag const& b) -> bool {
            return a.intersections.size() < b.intersections.size();
        });
        //

        for (int i = 0; i < m_quadtree.size(); ++i) {
            std::vector<Quadable*> potential_quads;
            auto& intersect_quads = m_quadtree[i].intersections;

            collisionData collision_data;
            for (int q = 0; q < intersect_quads.size(); ++q) {
                Collider* a_collider = static_cast<Collider*>(intersect_quads[q]);
                PhysicsBody& a_pb = *a_collider->m_pb_handler;
                Transform& a_tr = *a_collider->m_transform_handler;

                a_collider->m_cached_already_resolved = true;
                 
                potential_quads.clear();
                for (int j = i; j < m_quadtree.size(); ++j) 
                    if (m_tags_check[m_quadtree[j].tag].test(a_pb.tag))
                        m_quadtree[j].quadtree->get(a_pb.m_collider, potential_quads);

                for (auto& quad : potential_quads) {
                    Collider* collider_child = static_cast<Collider*>(quad);
                    if (quad != a_pb.m_collider) {
                        if (collider_child->m_cached_already_resolved)
                            continue;

                        a_pb.m_collider->check_collision(collider_child, collision_data);
                        if (collision_data.is_collide) {
                            // Start custom handlers
                            for (auto on_collide : a_pb.m_on_collide_handlers)
                                on_collide(*a_pb.m_collider->m_pb_handler, *collider_child->m_pb_handler, collision_data);
                            for (auto on_collide : collider_child->m_pb_handler->m_on_collide_handlers)
                                on_collide(*collider_child->m_pb_handler, *a_pb.m_collider->m_pb_handler, collision_data);

                            // Solve collisions
                            PhysicsBody& b_pb = *collider_child->m_pb_handler;
                            Transform& b_tr = b_pb.scene->get_component<Transform>(b_pb.entity);
                            if (
                                a_pb.type == PhysicsBody::pbType::RIGID &&
                                b_pb.type == PhysicsBody::pbType::RIGID
                            )   collision_data.distanse /= 2;

                            glm::vec3 norm = collision_data.normal;
                            glm::vec3 vel_diff = a_pb.velocity - b_pb.velocity;

                            if (a_pb.type == PhysicsBody::pbType::RIGID) {
                                float m_ratio;
                                if (b_pb.type == PhysicsBody::pbType::SOLID)
                                    m_ratio = (1.0 + a_pb.bouncyness) / 2.0;
                                else m_ratio = b_pb.m / (a_pb.m + b_pb.m);

                                a_tr.position -= norm * (collision_data.distanse + 0.01f);
                                a_pb.velocity = a_pb.velocity - 2.0f * m_ratio * glm::dot(vel_diff, norm) / glm::dot(norm, norm) * norm;
                                if (b_pb.type == PhysicsBody::pbType::SOLID)
                                    a_pb.force += -a_pb.friction * glm::dot(a_pb.m_last_force, norm) * glm::normalize(a_pb.velocity - glm::dot(a_pb.velocity, norm) * norm);
                            }
                            if (b_pb.type == PhysicsBody::pbType::RIGID) {
                                float m_ratio;
                                if (a_pb.type == PhysicsBody::pbType::SOLID)
                                    m_ratio = (1.0 + b_pb.bouncyness) / 2.0;
                                else m_ratio = a_pb.m / (a_pb.m + b_pb.m);

                                b_tr.position += norm * (collision_data.distanse + 0.01f);
                                b_pb.velocity = b_pb.velocity - 2.0f * m_ratio * glm::dot(-vel_diff, -norm) / glm::dot(norm, norm) * (-norm);
                                if (a_pb.type == PhysicsBody::pbType::SOLID) 
                                    b_pb.force += -b_pb.friction * glm::dot(b_pb.m_last_force, -norm) * glm::normalize(b_pb.velocity - glm::dot(b_pb.velocity, norm) * norm);
                            }
                        }
                    }
                }
            }
        }
    }
};