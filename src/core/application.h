#pragma once
#include "ogl.h"
#include "log.h"
#include "scene.h"
#include "timer.h"

struct ApplicationProperties {
    const char* title;
    int width;
    int height;
};

class Application {
private:
    static Application* m_instance;

    GLFWwindow* m_window;
    ApplicationProperties m_properties;

    Scene* m_scene;
public:
    Application(int width, int height, const char* title);
    ~Application();

    void run(Scene* scene);
    void go_to_scene(Scene* scene);
};