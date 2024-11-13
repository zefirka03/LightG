#version 430

struct envField {
    vec2 v;
    vec2 p;
    //float rho;
    //float _padding[3];
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

const float viscosity = 15;
const float d = 0.01;

envField u(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    out_f.p = vec2(0);
    //out_f.rho = 1;
    if(i < 0 || j < 0 || i >= size || j >= size)
        return out_f;

    return map[i * size + j];
}

vec2 f2texRECTbilerp(vec2 s)
{
    float4 st;
    st.xy = floor(s - 0.5) + 0.5;
    st.zw = st.xy + 1;
    
    vec2 t = s - st.xy; //interpolating factors
    
    vec2 tex11 = texRECT(st.xy);
    vec2 tex21 = texRECT(st.zy);
    vec2 tex12 = texRECT(st.xw);
    vec2 tex22 = texRECT(st.zw);

    return mix(mix(tex11, tex21, t.x), mix(tex12, tex22, t.x), t.y);
}

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);

    envField u_ij = u(pixelPos.x, pixelPos.y);

    //out_map[pixelPos.y + pixelPos.x * size].v = u_ij.v + d_t * ( 
    //    -( 
    //        u_ij.v.x * (u(pixelPos.x + 1, pixelPos.y).v - u(pixelPos.x - 1, pixelPos.y).v) / (2.f * d_h) + 
    //        u_ij.v.y * (u(pixelPos.x, pixelPos.y + 1).v - u(pixelPos.x, pixelPos.y - 1).v) / (2.f * d_h)
    //    )  + 
    //    viscosity * (
    //        (u(pixelPos.x + 1, pixelPos.y).v - 2 * u_ij.v + u(pixelPos.x - 1, pixelPos.y).v) / (d_h * d_h) + 
    //        (u(pixelPos.x, pixelPos.y + 1).v - 2 * u_ij.v + u(pixelPos.x, pixelPos.y - 1).v) / (d_h * d_h)
    //    )
    //    -(1.f / rho) * (
    //        (u(pixelPos.x + 1, pixelPos.y).p - u(pixelPos.x - 1, pixelPos.y).p) / (2.f * d_h) + 
    //        (u(pixelPos.x, pixelPos.y + 1).p - u(pixelPos.x, pixelPos.y - 1).p) / (2.f * d_h)
    //    )
    //)
    //;

    vec2 prev = pixelPos - d_t * u_ij.v;



    if(length(out_map[pixelPos.y + pixelPos.x * size].v) < 0.001)
        out_map[pixelPos.y + pixelPos.x * size].v = vec2(0);

    out_map[pixelPos.y + pixelPos.x * size].p = vec2(0);
}