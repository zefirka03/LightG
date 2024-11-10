~~vertex~~
#version 430 core

struct grass{
    vec3 position;
    float _padding;
};

layout (location = 0) in vec3 position;

layout (std430, binding = 0) buffer positionsBuffer {
  grass grassPositions[];
};

uniform mat4 camera;

void main() {
    gl_Position = camera * vec4(position * 20000 + grassPositions[gl_InstanceID].position, 1.0);
}

~~fragment~~
#version 430 core

out vec4 FragColor;

void main() {
    FragColor = vec4(0, 1, 0, 1);
}