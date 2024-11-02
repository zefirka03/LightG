#version 430

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D colorbuffer;

struct Child {
    int child_next;
    int child_prev;
    int node_it;
    int data[16];
};

layout(std430, binding = 1) buffer Childs {
    Child childs[];
};

void main() {

    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 screen_size = imageSize(colorbuffer);
    if (pixelPos.x >= screen_size.x || pixelPos.y >= screen_size.y) {
        return;
    }

    vec4 ncol = vec4(intBitsToFloat(childs[0].data[0]), intBitsToFloat(childs[0].data[1]), intBitsToFloat(childs[0].data[2]), 1.0);
    
    imageStore(colorbuffer, pixelPos, ncol);
}