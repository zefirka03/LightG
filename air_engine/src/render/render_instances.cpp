#include "render_instances.h"

QuadRenderInstance::QuadRenderInstance(
        glm::vec3 position,
        glm::vec3 origin,
        glm::vec3 rotation,
        glm::vec2 size,
        GLuint texture_id,
        glm::vec4 texture_rect
 ) : position(position), 
     origin(origin), 
     rotation(rotation), 
     size(size), 
     texture_id(texture_id),
     texture_rect(texture_rect) {}

std::array<vertex, 6> QuadRenderInstance::get_vertices() const  {
    glm::mat4 rotation_mat = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
    glm::vec4 origin4 = glm::vec4(origin, 0);
    glm::vec4 position4 = glm::vec4(position, 0);
    
    glm::vec3 a_pos = position4 + rotation_mat * (glm::vec4(0, 0, 0, 1)             - origin4);
    glm::vec3 b_pos = position4 + rotation_mat * (glm::vec4(0, size.y, 0, 1)        - origin4);
    glm::vec3 c_pos = position4 + rotation_mat * (glm::vec4(size.x, 0, 0, 1)        - origin4);
    glm::vec3 d_pos = c_pos + b_pos - a_pos;

    return {
        vertex{a_pos, glm::vec2(texture_rect.x, texture_rect.y)},
        vertex{b_pos, glm::vec2(texture_rect.x, texture_rect.w)},
        vertex{c_pos, glm::vec2(texture_rect.z, texture_rect.y)},
        vertex{b_pos, glm::vec2(texture_rect.x, texture_rect.w)},
        vertex{c_pos, glm::vec2(texture_rect.z, texture_rect.y)},
        vertex{d_pos, glm::vec2(texture_rect.z, texture_rect.w)}
    };
}