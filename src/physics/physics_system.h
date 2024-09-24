#pragma once
#include "../core/core.h"
#include "physics_core.h"

struct PhysicsBody : public Component {
    Collider* collider;
};

class PhysicsSystem : public System {
private:
    void init() override {

    }

    void start() override {

    }
    
    void update(float delta_time) override {

    }
};