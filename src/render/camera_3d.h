#pragma once
#include "../core/air_engine.h"

struct Projection{
    virtual glm::mat4 get_mat() const = 0;
};

struct Ortho : public Projection{
    int width;
    int height;

    Ortho(int _width, int _height)
        : width(_width), height(_height) {}

    glm::mat4 get_mat() const override {
        return glm::ortho(0.f, (float)width, 0.f, (float)height);
    }
};

struct Perspective : public Projection{
    int width;
    int height;
    float fov;
    float near = 1e-1;
    float far = 1e4;

    Perspective(int _width, int _height, float _fov) 
        : width(_width), height(_height), fov(_fov) {}
    Perspective(int _width, int _height, float _fov, float _near, float _far) 
        : width(_width), height(_height), fov(_fov), near(_near), far(_far) {}

    glm::mat4 get_mat() const override {
        return glm::perspective(fov, (float)width/(float)height, near, far);
    }
};

class Camera3d : public Component{
private:
    bool m_is_main;
    glm::mat4 m_projection_cache;
    Projection* proj;

    void _reprojection(){
        m_projection_cache = proj->get_mat();
    }
public:
    Camera3d(Projection* projection, bool is_main = true){
        proj = projection;
        m_is_main = is_main;
        
        _reprojection();
    }
    ~Camera3d(){}

    glm::mat4 get_projection() const {
        return m_projection_cache;
    }

    bool is_main() const {
        return m_is_main;
    }
};