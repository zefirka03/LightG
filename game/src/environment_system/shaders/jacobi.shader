#version 430

struct envField {
    vec2 v;
};

layout(local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 0) buffer Map {
    envField map[];
};

layout(std430, binding = 1) buffer OutMap {
    envField out_map[];
};

uniform int size;
uniform float d_t;
const float d_h = 2;
const float viscosity = 100;

envField u_out(ivec2 pos){
    envField out_f;
    out_f.v = vec2(0.2, 0.5);
    if(pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size)
        return out_f;

    return out_map[pos.x * size + pos.y];
}

int IX(int i, int j){
    return i * size + j;
}

vec2 jacobi(int i, int j, float a, vec2 b) {
    ivec2 coords = ivec2(i, j);

    vec2 vL = u_out(coords - ivec2(1, 0)).v;
    vec2 vR = u_out(coords + ivec2(1, 0)).v;
    vec2 vB = u_out(coords - ivec2(0, 1)).v;
    vec2 vT = u_out(coords + ivec2(0, 1)).v;
 
    return (1.f / (1 + 4.f * a / (d_h * d_h))) * (b + a / (d_h * d_h) * (vL + vR + vB + vT));
}

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    int ix = IX(pixelPos.x, pixelPos.y);

    float a = viscosity * d_t;
    out_map[ix].v = jacobi(
        pixelPos.x,
        pixelPos.y,
        a,
        map[ix].v
    );
}