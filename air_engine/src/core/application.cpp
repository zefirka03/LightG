#include "application.h"

Application* Application::m_instance = nullptr;

Application::Application(int width, int height, const char* title){
    m_scene = nullptr;

    AIR_ASSERT(!m_instance, "Application instance already exist");
    m_instance = this;

    int success = glfwInit();
    AIR_ASSERT(glfwInit(), "GLFW not inited");


    glfwWindowHint(GLFW_SAMPLES, 8);
    glEnable(GL_MULTISAMPLE);

    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(m_window);

    success = glewInit() == GLEW_OK;
    AIR_ASSERT(success, "GLEW not inited");

    m_properties.width = width;
    m_properties.height = height;
    m_properties.title = title;

    glfwSwapInterval(0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

Application::~Application(){
    glfwTerminate();
}

void Application::run(Scene* scene){
    Timer timer_delta_time;

    go_to_scene(scene);
    while(!glfwWindowShouldClose(m_window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        scene->_update(timer_delta_time.delta());

        glfwSwapBuffers(m_window);
    }
}

void Application::go_to_scene(Scene* scene){
    if(m_scene) delete m_scene;
    m_scene = scene;
    scene->_init();
    scene->_start();
}

Application& Application::get_instance(){
    return *m_instance;
}

GLFWwindow* Application::get_native_window() const {
    return m_window;
}

ApplicationProperties const& Application::get_properties() const {
    return m_properties;
}