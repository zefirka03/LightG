#pragma once
#include "../core/air_engine.h"

class Script;

class ScriptInstance {
friend class ScriptingSystem;
protected:
    virtual void start() {}
    virtual void update(float delta_time) {}

    inline Scene& get_scene() const {
        return **m_scene;
    }

    inline Entity get_game_object() const {
        return *m_game_object;
    }
private:
friend class Script;
    Scene** m_scene;
    Entity* m_game_object;
};

class Script : public Component {
public:
    Script(ScriptInstance* instance) {
        m_instance = instance;
        m_instance->m_scene = &scene;
        m_instance->m_game_object = &game_object;
    }

    ~Script() {
        delete m_instance;
    }
private:
friend class ScriptingSystem;
    ScriptInstance* m_instance;
};

class ScriptingSystem : public System {
private:
    void init() override {
        auto view_scripts = m_registry->view<Script>();
        view_scripts.each([&](Script& script){
            script.m_instance->start();
        });
    }

    void update(float delta_time) override {
        auto view_scripts = m_registry->view<Script>();
        view_scripts.each([&](Script& script){
            script.m_instance->update(delta_time);
        });
    }
};