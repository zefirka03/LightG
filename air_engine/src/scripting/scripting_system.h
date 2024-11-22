#pragma once
#include "../core/core.h"

class Script {
friend class ScriptingSystem;
friend class ScriptComponent;
protected:
    virtual void start() {}
    virtual void update(float delta_time) {}

    inline Scene& get_scene() const {
        return *m_scene;
    }

    inline Entity get_entity() const {
        return m_entity;
    }

    virtual ~Script() {}
private:
    Scene* m_scene;
    Entity m_entity;
};

class ScriptComponent : public Component {
public:
    ScriptComponent() {}

    ScriptComponent(ScriptComponent&& sc) noexcept {
        m_script_instances = sc.m_script_instances;
    }

    template<class Script_t, class ...Args>
    Script_t* bind(Args&&... args) {
        static_assert(std::is_base_of<Script, Script_t>::value, "Script_t class must be derived by Script");

        Script_t* t_script = new Script_t(args...);
        t_script->m_scene = scene;
        t_script->m_entity = entity;

        m_script_instances.push_back(t_script);
        m_script_instances.back()->start();

        return t_script;
    }

    ~ScriptComponent() {
        for (size_t i = 0; i < m_script_instances.size(); ++i)
            delete m_script_instances[i];
    }
private:
friend class ScriptingSystem;
    std::vector<Script*> m_script_instances;

    void _update(float delta_time) {
        for (Script* script_instance : m_script_instances)
            script_instance->update(delta_time);
    }
};

class ScriptingSystem : public System {
private:
    void init() override;
    void start() override;
    void update(float delta_time) override;
};