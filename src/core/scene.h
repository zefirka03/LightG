#pragma once
#include "ecs.h"
#include <vector>

class Scene{
private:
friend class Application;
    std::vector<System*> m_systems;
    entt::registry m_registry;
    
    void _update(float delta_time) {
        on_update(delta_time);

        for(System* t_system : m_systems)
            t_system->update(delta_time);
    };

    void _init() {
        on_init();

        for (System* t_system : m_systems)
            t_system->init();
    }

public:
    Scene() = default;
    virtual ~Scene() {
        for(System* t_system : m_systems)
            delete t_system;
    }

    template<class System_t, class ...Args>
    System_t* add_system(Args... args){
        static_assert(std::is_base_of<System, System_t>::value, "System_t class must be derived by System");

        System_t* t_system = new System_t(std::forward(args)...);
        t_system->m_registry = &m_registry;
        m_systems.emplace_back(t_system);
        return t_system;
    }

    Entity create_entity(){
        return Entity(m_registry.create());
    }

    template<class Component_t, class ...Args>
	Component_t& add_component(Entity entity, Args&&... args){
        static_assert(std::is_base_of<Component, Component_t>::value, "Component_t class must be derived by Component");

		auto& it = m_registry.emplace<Component_t>(entity.m_id, std::forward<Args>(args)...);
		it.entity = Entity(entity.m_id);
        it.scene = this;
		return it;
    }

    template<class Component_t>
    void remove_component(Entity entity) {
        m_registry.remove<Component_t>(entity.m_id);
    }

    template<class Component_t>
    Component_t& get_component(Entity entity) {
        return m_registry.get<Component_t>(entity.m_id);
    }

    template<class ...Component_ts>
    bool has_component(Entity entity) {
        return m_registry.all_of<Component_ts...>(entity.m_id);
    }

protected:
    virtual void on_init() {}
    virtual void on_start() {}
    virtual void on_update(float delta_time) {}
};