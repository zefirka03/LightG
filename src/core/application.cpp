#include "application.h"

Application* Application::m_instance = nullptr;

Application::Application(int width, int height, const char* title){
    AIR_ASSERT(!m_instance, "Application instance already exist");
    m_instance = this;

    int success = glfwInit();
    AIR_ASSERT(glfwInit(), "GLFW not inited");

    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(m_window);

    success = glewInit() == GLEW_OK;
    AIR_ASSERT(success, "GLEW not inited");

    m_properties.width = width;
    m_properties.height = height;
    m_properties.title = title;
}

Application::~Application(){
    if(m_window)
        glfwTerminate();
}

void Application::run(){
    while(!glfwWindowShouldClose(m_window)){
        glfwPollEvents();

        glfwSwapBuffers(m_window);
    }
}