#pragma once
#include "../core/core.h"
#include "transform.h"

struct Projection {
    virtual glm::mat4 get_mat() const = 0;
};

struct Ortho : public Projection {
    float width;
    float height;
    float near = 1e-2f;
    float far = 1e4f;

    Ortho(float _width, float _height)
        : width(_width), height(_height) {}
    Ortho(float _width, float _height, float _near, float _far)
        : width(_width), height(_height), near(_near), far(_far) {}

    glm::mat4 get_mat() const override {
        return glm::ortho(0.f, width, 0.f, height, near, far);
    }
};

struct Perspective : public Projection {
    int width;
    int height;
    float fov;
    float near = 1e-2f;
    float far = 1e4f;

    Perspective(int _width, int _height, float _fov) 
        : width(_width), height(_height), fov(_fov) {}
    Perspective(int _width, int _height, float _fov, float _near, float _far) 
        : width(_width), height(_height), fov(_fov), near(_near), far(_far) {}

    glm::mat4 get_mat() const override {
        return glm::perspectiveFov(fov, (float)width, (float)height, near, far);
    }
};

class Camera3d : public Component {
private:
    bool m_is_main;

    glm::mat4 m_projection_cache;
    Projection* m_proj;

    void _reprojection(){
        m_projection_cache = m_proj->get_mat();
    }
public:
    float yaw = 0;
    float roll = 0;

    Camera3d(Projection* projection, bool is_main = true) {
        m_proj = projection;
        m_is_main = is_main;

        _reprojection();
    }
    ~Camera3d(){
        delete m_proj;
    }

    glm::mat4 get_projection() const {
        return m_projection_cache;
    }

    glm::vec3 get_forward() const {
        return glm::vec3(
            cos(roll) * sin(yaw),
            sin(roll), 
            cos(roll) * cos(yaw)
        );
    }

    glm::mat4 get_view() const {
        auto& transform = scene->get_component<Transform>(entity);
        
        return glm::lookAt(transform.position, transform.position + get_forward(), glm::vec3(0, 1, 0));
    }

    bool is_main() const {
        return m_is_main;
    }
};