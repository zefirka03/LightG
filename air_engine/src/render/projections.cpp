#include "camera_3d.h"

Ortho::Ortho(float _width, float _height)
    : width(_width), height(_height) {}

Ortho::Ortho(float _width, float _height, float _near, float _far)
    : width(_width), height(_height), near(_near), far(_far) {}

glm::mat4 Ortho::get_mat() const {
    return glm::ortho(0.f, width, 0.f, height, near, far);
}

///////////////

Perspective::Perspective(int _width, int _height, float _fov) 
    : width(_width), height(_height), fov(_fov) {}

Perspective::Perspective(int _width, int _height, float _fov, float _near, float _far) 
    : width(_width), height(_height), fov(_fov), near(_near), far(_far) {}

glm::mat4 Perspective::get_mat() const {
    return glm::perspectiveFov(fov, (float)width, (float)height, near, far);
}