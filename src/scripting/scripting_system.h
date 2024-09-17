#pragma once
#include "../core/air_engine.h"

class ScriptComponent;

class Script {
friend class ScriptingSystem;
protected:
    virtual void start() {}
    virtual void update(float delta_time) {}

    inline Scene& get_scene() const {
        return *m_scene;
    }

    inline Entity get_game_object() const {
        return m_game_object;
    }
private:
friend class ScriptComponent;
    Scene* m_scene;
    Entity m_game_object;
};

class ScriptComponent : public Component {
public:
    template<class Script_t, class ...Args>
    void bind(Args&&... args) {
        static_assert(std::is_base_of<Script, Script_t>::value, "Script_t class must be derived by Script");

        Script_t* t_script = new Script_t(args...);
        t_script->m_scene = scene;
        t_script->m_game_object = game_object;

        m_script_instances.push_back(t_script);
    }

    ~ScriptComponent() {
        for (size_t i = 0; i < m_script_instances.size(); ++i)
            delete m_script_instances[i];
    }
private:
friend class ScriptingSystem;
    std::vector<Script*> m_script_instances;

    void _start() {
        for (Script* script_instance : m_script_instances)
            script_instance->start();
    }

    void _update(float delta_time) {
        for (Script* script_instance : m_script_instances)
            script_instance->update(delta_time);
    }
};

class ScriptingSystem : public System {
private:
    void init() override {
        auto view_scripts = m_registry->view<ScriptComponent>();
        view_scripts.each([&](ScriptComponent& script){
            script._start();
        });
    }

    void update(float delta_time) override {
        auto view_scripts = m_registry->view<ScriptComponent>();
        view_scripts.each([&](ScriptComponent& script) {
            script._update(delta_time);
        });
    }
};