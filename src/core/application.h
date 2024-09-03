#pragma once
#include "ogl.h"
#include "log.h"

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
public:
    Application(int width, int height, const char* title);
    ~Application();

    void run();
};