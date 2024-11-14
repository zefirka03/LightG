#version 430

struct envField {
    vec2 v;
    float s;
    float density;
};

layout(local_size_x = 16, local_size_y = 16) in;
layout(std430, binding = 0) buffer Map {
    envField map[];
};

layout(std430, binding = 1) buffer OutMap {
    envField out_map[];
};

const int size = 128;

uniform float d_t;
const float d_h = 0.5;
const float rho = 1;

const float viscosity = 1;

envField u(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    out_f.s = 0;
    out_f.density = 0;
    if(i < 0 || j < 0 || i >= size || j >= size)
        return out_f;

    return map[i * size + j];
}

envField u(ivec2 pos){
    envField out_f;
    out_f.v = vec2(0);
    out_f.s = 0;
    out_f.density = 0;
    if(pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size)
        return out_f;

    return map[pos.x * size + pos.y];
}

envField u_out(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    out_f.s = 0;
    out_f.density = 0;
    if(i < 0 || j < 0 || i >= size || j >= size)
        return out_f;

    return out_map[i * size + j];
}


int IX(int i, int j){
    return i * size + j;
}


void diffuse(int i, int j, float dt, int iter){
    vec2 disp = vec(i, j) - dt * u(i, j).v / (25000.f / float(size));

    ivec4 poses;
    poses.x = floor(disp.x - 0.5); 
    poses.y = floor(disp.y - 0.5);
    poses.zw = poses.xy + 1;

    vec2 t = disp - poses.xy;
    out_map[IX(i, j)].v = mix(mix(u(poses.xy).v, u(poses.xz).v, t.x), mix(u(poses.yz).v, u(poses.wz).v, t.x), t.y);
}

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);

    diffuse(pixelPos.x, pixelPos.y, d_t, 4);

}