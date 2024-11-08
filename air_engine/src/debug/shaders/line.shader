~~vertex~~
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 camera;
out vec4 o_color;

void main() {
	gl_Position = camera * vec4(position, 1.0); 
    o_color = color;
}
 
~~fragment~~
#version 430 core

in vec4 o_color;
out vec4 out_color;

void main() {
	out_color = o_color;
}