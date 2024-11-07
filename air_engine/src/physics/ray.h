#pragma once
#include "../core/core.h"
#include "bounding_box.h"

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

    // Slab method
    bool intersect(boundingBox& bbox, float& t) const {
        glm::vec3 a = (bbox.get_a() - origin) / direction;
        glm::vec3 b = (bbox.get_b() - origin) / direction;

        t = std::max(
            std::max(
                std::min(a.x, b.x),
                std::min(a.y, b.y)
            ),  std::min(a.z, b.z)
        );

        float tmax = std::min(
            std::min(
                std::max(a.x, b.x),
                std::max(a.y, b.y)
            ),  std::max(a.z, b.z)
        );
        //printf("Tmax: %f, Tmin: %f\n", tmax, t);
        if (tmax >= std::max(0.0f, t) && t < length)
            return true;
        return false;
    }
};