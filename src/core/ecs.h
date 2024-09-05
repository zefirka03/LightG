#pragma once
#include <entt/entt.hpp>

class Entity{
private:
friend class Scene;
    entt::entity m_id = entt::null;

    Entity(entt::entity id) : m_id(id) {}
public:
    Entity() = default;

    void operator=(Entity other){
        m_id = other.m_id;
    }
};

class Component{
public:
    Entity game_object;
};

class System{
protected:
friend class Scene;
    entt::registry* m_registry;
public:
    virtual void init() {}
    virtual void update(float delta_time) {}
};