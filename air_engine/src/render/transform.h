#pragma once
#include "../core/core.h"

struct Transform : public Component {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 origin = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
};