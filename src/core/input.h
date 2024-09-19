#pragma once
#include "ogl.h"
#include "application.h"
#include "keycodes.h"

class Input {
public:
	static bool is_key_pressed(KeyCode key);
	static bool is_mouse_button_pressed(MouseCode button);
	static glm::vec2 get_mouse_position();
	static void set_cursor_mode(CursorMode mode);
	static float get_mouse_x();
	static float get_mouse_y();
};