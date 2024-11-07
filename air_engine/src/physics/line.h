#pragma once
#include "../core/core.h"

struct line2{
    glm::vec2 a;
    glm::vec2 b;
};

struct intersection{
    bool is_intersect;
    glm::vec2 point;
};

intersection is_lines_intersect(line2 const& a, line2 const& b);