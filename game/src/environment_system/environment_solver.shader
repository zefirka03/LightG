#version 430

struct envField {
    float x;
    float z;
};

layout(local_size_x = 16, local_size_y = 16) in;
layout(std430, binding = 0) buffer Map {
    envField map[];
};

layout(std430, binding = 1) buffer OutMap {
    envField out_map[];
};

uniform float time;

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    out_map[pixelPos.y * 256 + pixelPos.x].x = sin(pixelPos.y / 100.f + time + pixelPos.x / 25.f);
}