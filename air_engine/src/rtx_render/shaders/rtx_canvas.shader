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

uniform sampler2D textureSamp;

void main() {
	out_color = texture(textureSamp, o_texCoord);
}