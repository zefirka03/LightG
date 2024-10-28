#pragma once
#include "../core/core.h"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    float length;

    Ray() {}

    Ray(
        glm::vec3 _origin, 
        glm::vec3 _direction, 
        float _lenght
    ) : origin(_origin), direction(_direction), length(_lenght) {}
};