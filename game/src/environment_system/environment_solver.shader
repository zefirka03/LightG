#version 430

struct envField {
    vec2 v;
    vec2 p;
};

layout(local_size_x = 16, local_size_y = 16) in;
layout(std430, binding = 0) buffer Map {
    envField map[];
};

layout(std430, binding = 1) buffer OutMap {
    envField out_map[];
};

uniform float d_t;
const float d_h = 1;

const float viscosity = 1;
const float pho = 1;

envField u(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    out_f.p = vec2(0);
    if(i < 0 || j < 0 || i >= 64 || j >= 64)
        return out_f;

    return map[i * 64 + j];
}

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);

    envField u_ij = u(pixelPos.x, pixelPos.y);

    out_map[pixelPos.y + pixelPos.x * 64].v = u_ij.v + d_t * ( 
        -( 
            u_ij.v.x * (u(pixelPos.x + 1, pixelPos.y).v - u(pixelPos.x - 1, pixelPos.y).v) / (2.f * d_h) + 
            u_ij.v.y * (u(pixelPos.x, pixelPos.y + 1).v - u(pixelPos.x, pixelPos.y - 1).v) / (2.f * d_h)
        )  + 
        viscosity * (
            (u(pixelPos.x + 1, pixelPos.y).v - 2 * u_ij.v + u(pixelPos.x - 1, pixelPos.y).v) / (d_h * d_h) + 
            (u(pixelPos.x, pixelPos.y + 1).v - 2 * u_ij.v + u(pixelPos.x, pixelPos.y - 1).v) / (d_h * d_h)
        )
        -(1.f / pho) * (
            (u(pixelPos.x + 1, pixelPos.y).p - u(pixelPos.x - 1, pixelPos.y).p) / (2.f * d_h) + 
            (u(pixelPos.x, pixelPos.y + 1).p - u(pixelPos.x, pixelPos.y - 1).p) / (2.f * d_h)
        )
    )
    ;

    if(length(out_map[pixelPos.y + pixelPos.x * 64].v) < 0.001)
        out_map[pixelPos.y + pixelPos.x * 64].v = vec2(0);

    out_map[pixelPos.y + pixelPos.x * 64].p = vec2(0);
}