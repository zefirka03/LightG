#include "application.h"

Application* Application::m_instance = nullptr;

Application::Application(int width, int height, const char* title){
    m_scene = nullptr;

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
    glfwTerminate();
}

void Application::run(Scene* scene){
    Timer timer_delta_time;

    go_to_scene(scene);
    while(!glfwWindowShouldClose(m_window)){
        glfwPollEvents();
        scene->_update(timer_delta_time.delta());

        glfwSwapBuffers(m_window);
    }
}

void Application::go_to_scene(Scene* scene){
    if(m_scene) delete m_scene;
    m_scene = scene;
    scene->on_start();
}