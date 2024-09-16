~~vertex~~
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 o_texCoord;
uniform mat4 camera;

void main() {
	gl_Position = camera * vec4(position, 1.0); 
    o_texCoord = texCoord;
}
 
~~fragment~~
#version 430 core

out vec4 out_color;
in vec2 o_texCoord;

uniform mat4 camera_rtx;

layout(std430, binding = 3) buffer SceneLayout
{
    float Scene[];
};

void main() {
	out_color = vec4(o_texCoord * vec2(Scene[0], Scene[1]), 1, 1);
}