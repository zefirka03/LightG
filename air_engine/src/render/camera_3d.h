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

    Ortho(float _width, float _height);
    Ortho(float _width, float _height, float _near, float _far);

    glm::mat4 get_mat() const override;
};

struct Perspective : public Projection {
    int width;
    int height;
    float fov;
    float near = 1e-2f;
    float far = 1e4f;

    Perspective(int _width, int _height, float _fov);
    Perspective(int _width, int _height, float _fov, float _near, float _far);

    glm::mat4 get_mat() const override;
};

class Camera3d : public Component {
private:
    bool m_is_main;

    glm::mat4 m_projection_cache;
    Projection* m_proj;

    void _reprojection();
public:
    float yaw = 0;
    float roll = 0;

    Camera3d(Projection* projection, bool is_main = true);
    ~Camera3d();

    glm::mat4 get_projection() const;
    glm::vec3 get_forward() const;
    glm::mat4 get_view() const;
    bool is_main() const;
};