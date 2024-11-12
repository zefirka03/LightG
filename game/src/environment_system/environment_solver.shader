#version 430

struct envField {
    float x;
    float z;
};

layout(local_size_x = 8, local_size_y = 8) in;
layout(std430, binding = 0) buffer Map {
    envField map[];
};

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    map[pixelPos.y * 256 + pixelPos.x].x = sin(pixelPos.y / 100.f + pixelPos.x / 25.f);
}