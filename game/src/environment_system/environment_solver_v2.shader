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

const int size = 128;

uniform float d_t;
const float d_h = 0.5;
const float rho = 1;

const float viscosity = 20;

envField u(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    if(i < 0 || j < 0 || i >= size || j >= size)
        return out_f;

    return map[i * size + j];
}

envField u(ivec2 pos){
    envField out_f;
    out_f.v = vec2(0);
    if(pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size)
        return out_f;

    return map[pos.x * size + pos.y];
}

envField u_out(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    if(i < 0 || j < 0 || i >= size || j >= size)
        return out_f;

    return out_map[i * size + j];
}

envField u_out(ivec2 pos){
    envField out_f;
    out_f.v = vec2(0);
    if(pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size)
        return out_f;

    return out_map[pos.x * size + pos.y];
}

int IX(int i, int j){
    return i * size + j;
}

//void diffuse(int i, int j, float dt){
//    vec2 disp = vec2(i, j) - dt * u(i, j).v / (25000.f / float(size));
//
//    ivec4 poses;
//    poses.x = int(floor(disp.x - 0.5)); 
//    poses.y = int(floor(disp.y - 0.5));
//    poses.zw = poses.xy + 1;
//
//    vec2 t = disp - poses.xy;
//
//    float tex11 = u(poses.xy).density;
//    float tex21 = u(poses.zy).density;
//    float tex12 = u(poses.xw).density;
//    float tex22 = u(poses.zw).density;
// 
//    out_map[IX(i, j)].density = mix(mix(tex11, tex21, t.x), mix(tex12, tex22, t.x), t.y);
//}

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

    out_map[ix].v = map[ix].v;
    barrier();

    float a = viscosity * d_t;
    vec2 curr_b = map[ix].v;
    for(int i=0; i < 8; ++i){
        out_map[ix].v = jacobi(
            pixelPos.x,
            pixelPos.y,
            a,
            curr_b
        );
        barrier();
    }
}