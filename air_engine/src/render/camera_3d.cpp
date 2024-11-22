#include "camera_3d.h"

void Camera3d::_reprojection(){
    m_projection_cache = m_proj->get_mat();
}

Camera3d::Camera3d(Projection* projection, bool is_main) {
    m_proj = projection;
    m_is_main = is_main;
    _reprojection();
}

Camera3d::~Camera3d(){
    delete m_proj;
}

glm::mat4 Camera3d::get_projection() const {
    return m_projection_cache;
}

glm::vec3 Camera3d::get_forward() const {
    return glm::vec3(
        cos(roll) * sin(yaw),
        sin(roll), 
        cos(roll) * cos(yaw)
    );
}

glm::mat4 Camera3d::get_view() const {
    auto& transform = scene->get_component<Transform>(entity);
    
    return glm::lookAt(transform.position, transform.position + get_forward(), glm::vec3(0, 1, 0));
}

bool Camera3d::is_main() const {
    return m_is_main;
}