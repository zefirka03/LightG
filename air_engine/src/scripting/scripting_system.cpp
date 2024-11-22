#include "scripting_system.h"

void ScriptingSystem::init() {}

void ScriptingSystem::start() {}

void ScriptingSystem::update(float delta_time) {
    auto view_scripts = m_registry->view<ScriptComponent>();
    view_scripts.each([&](ScriptComponent& script) {
        script._update(delta_time);
    });
}