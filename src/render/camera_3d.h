#pragma once
#include "../core/air_engine.h"

class Camera3d : public Component{
private:
    bool m_is_main;
    glm::mat4 m_projection;

    int m_width;
    int m_height;

    void _reprojection(){
        m_projection = glm::ortho(0.f, (float)m_width, 0.f, (float)m_height);
    }
public:
    Camera3d(int width, int height){
        m_width = width;
        m_height = height;
        
        _reprojection();
    }
    ~Camera3d(){}

    glm::mat4 get_projection() const {
        return m_projection;
    }
};