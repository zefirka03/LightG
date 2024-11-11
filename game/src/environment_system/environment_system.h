#pragma once
#include "core/core.h"

class EnvironmentSystem : public System {
public:
    int size = 256;
    unsigned char* map = nullptr;
private:

    void init() override {
        map = new unsigned char[size * size * 2];
        for (int i = 0; i < size * size; ++i) {
                map[2*i] = 0;

                map[2*i+1] = 255;
        }
    }
    
    void update(float delta_time) override {
        
    }
};