#pragma once
#include "ecs.h"
#include <typeindex>
#include <unordered_map>
#include <vector>

struct _DestroyedEntityComponent : public Component {};

class Scene{
private:
friend class Application;
    // to fast indexing
    std::unordered_map<std::type_index, System*> m_systems_index;
    // to keep right order
    std::vector<System*> m_systems;
    entt::registry m_registry;
    
    void _update(float delta_time) {
        // Call self update
        on_update(delta_time);

        // Call systems update
        for(auto& t_system : m_systems)
            t_system->update(delta_time);

        // Delete destroyed entities
        m_registry.view<_DestroyedEntityComponent>().each([&](auto const entity, _DestroyedEntityComponent& dec) {
            m_registry.destroy(entity);
        });
    }

    void _init() {
        on_init();

        for (auto& t_system : m_systems)
            t_system->init();
    }

    void _start() {
        on_start();

        for (auto& t_system : m_systems)
            t_system->start();
    }

public:
    Scene() = default;
    virtual ~Scene() {
        for(auto& t_system : m_systems)
            delete t_system;
    }

    Entity create_entity(){
        return Entity(m_registry.create());
    }

    void destroy_entity(Entity const& entity) {
        if (!has_component<_DestroyedEntityComponent>(entity))
            add_component<_DestroyedEntityComponent>(entity);
    }

    template<class Component_t>
    Entity get_entity_of_type() const {
        auto entts_view = m_registry.view<Component_t>();
        return Entity(entts_view.front());
    }

    template<class Component_t>
    std::vector<Entity> get_all_entities_of_type() const {
        std::vector<Entity> out;
        auto entts_view = m_registry.view<Component_t>();
        entts_view.each([&](const auto entity, Component_t const& comp) {
            out.emplace_back(Entity(entity));
        });
        return out;
    }

    template<class System_t, class ...Args>
    System_t* add_system(Args... args){
        static_assert(std::is_base_of<System, System_t>::value, "System_t class must be derived by System");

        System_t* t_system = new System_t(std::forward(args)...);
        static_cast<System*>(t_system)->m_scene = this;
        t_system->m_registry = &m_registry;
        m_systems_index.emplace(typeid(System_t), t_system);
        m_systems.emplace_back(t_system);
        return t_system;
    }

    template<class System_t>
    System_t* get_system() {
        auto iter = m_systems_index.find(typeid(System_t));

        if (iter != m_systems_index.end())
            return dynamic_cast<System_t*>(iter->second);
        return nullptr;
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