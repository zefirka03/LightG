#include "input.h"

bool Input::is_key_pressed(KeyCode key){
    auto state = glfwGetKey(Application::get_instance().get_native_window(), static_cast<int32_t>(key));
	return state == GLFW_PRESS;
}

bool Input::is_mouse_button_pressed(MouseCode button){
    auto state = glfwGetMouseButton(Application::get_instance().get_native_window(), static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

glm::vec2 Input::get_mouse_position(){
	double xpos, ypos;
	glfwGetCursorPos(Application::get_instance().get_native_window(), &xpos, &ypos);

	return { (float)xpos, (float)ypos };
}

float Input::get_mouse_x(){
    return get_mouse_position().x;
}

float Input::get_mouse_y(){
    return get_mouse_position().y;
}

