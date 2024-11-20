#version 430

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D colorbuffer;

#define PI 3.1415926535
#define TWO_PI 2 * PI

struct boundingBox {
    vec3 a;          // 12 байт
    float padding_1;  // 4 байта для выравнивания
    vec3 b;          // 12 байт
    float padding_2;  // 4 байта для выравнивания
    vec3 center;     // 12 байт
    float diameter;  // 4 байта
}; // Всего 64 байта

struct Child {
    int child_next;
    int child_prev;
    int node_it;
    int _padding;

    int object_type;
    int data[15];
};

struct Node {
    boundingBox bounds;
    int childs_size;
    int child_first;
    int child_last;
    int deep;
    int pool_position;
    bool is_devided;
    float _padding[2];
}; // Всего 80 байт

struct Ray {
    vec3 origin;
    vec3 direction;
    float length;
};

struct Camera {
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	vec3 cameraRight;
	float fov, dist;
};

struct Sphere {
    vec3 position;
    float radius;
};

struct Plane {
    vec3 position;
    vec3 origin;
    vec2 size;

    float rotation;
};

struct Sprite {
    vec3 position;
    vec3 origin;
    vec2 size;

    float rotation;
};

layout(std430, binding = 1) buffer Childs {
    Child childs[];
};

layout(std430, binding = 2) buffer Nodes {
    Node nodes[];
};

int get_pool_position(int i) {
    return 8 * i + 1;
}

int get_pool_position_up(int i) {
    return (i - 1) / 8;
}

bool contains(boundingBox box, vec3 point) {
	if (
		point.x >= box.a.x &&
		point.y >= box.a.y &&
		point.z >= box.a.z &&
		point.x <= box.b.x &&
		point.y <= box.b.y &&
		point.z <= box.b.z
	) return true;
	return false;
}

bool intersect(Ray ray, boundingBox bbox, inout float t) {
    vec3 a = (bbox.a - ray.origin) / ray.direction;
    vec3 b = (bbox.b - ray.origin) / ray.direction;

    t = max(
        max(
            min(a.x, b.x),
            min(a.y, b.y)
        ),  min(a.z, b.z)
    );

    float tmax = min(
        min(
            max(a.x, b.x),
            max(a.y, b.y)
        ),  max(a.z, b.z)
    );

    if (tmax >= max(0, t) && t < ray.length)
        return true;
    return false;
}

// It can be without branches
int get_down_index(int node_it, vec3 point) {
    vec3 middle = nodes[node_it].bounds.center;

    if (point.x > middle.x) {
        if (point.y > middle.y) {
            if (point.z > middle.z) {
                return 0;
            }
            else {
                return 1;
            }
        }
        else {
            if (point.z > middle.z) {
                return 2;
            }
            else {
                return 3;
            }
        }
    }
    else {
        if (point.y > middle.y) {
            if (point.z > middle.z) {
                return 4;
            }
            else {
                return 5;
            }
        }
        else {
            if (point.z > middle.z) {
                return 6;
            }
            else {
                return 7;
            }
        }
    }
}

bool intersect_sphere(Ray ray, Sphere sphere, inout float t, inout vec3 o_norm){
    float t0, t1;

    vec3 L = sphere.position - ray.origin;
    float tca = dot(L, ray.direction);
    // if (tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if (d2 > sphere.radius * sphere.radius) {
        return false;
    }

    float thc = sqrt(sphere.radius * sphere.radius - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    if (t0 > t1) {
        float tmp = t0;
        t0 = t1;
        t1 = tmp;
    }

    if (t0 < 0) {
        t0 = t1; // If t0 is negative, let's use t1 instead.
        if (t0 < 0) 
            return false;
    }
    if (t0 < ray.length) {
        vec3 inter = ray.origin + ray.direction * t0;

        t = t0;
        o_norm = normalize(inter - sphere.position);
        return true;
    }
    return false;
}

bool intersect_plane(Ray ray, Plane plane, inout float t, inout vec3 o_norm){
    vec3 n = vec3(0, 1, 0);
	t = dot(n, plane.position - ray.origin) / dot(n, ray.direction);
	if(t > ray.length || t < 0) return false;

	vec3 intersection_point = ray.origin + t * ray.direction;

	vec3 diff = intersection_point - plane.position;
	float x_proj = dot(vec3(cos(plane.rotation), 0, sin(plane.rotation)), diff);
	float z_proj = dot(vec3(sin(plane.rotation), 0, cos(plane.rotation)), diff);

	if (
        x_proj < plane.size.x - plane.origin.x && 
        x_proj > -plane.origin.x && 
        z_proj < plane.size.y - plane.origin.y && 
        z_proj > -plane.origin.y
    ) {
        o_norm = n;
		return true;
	}

	return false;
}

bool intersect_sprite(Ray ray, Sprite sprite, inout float t, inout vec3 o_norm){
    vec3 n = vec3(sin(sprite.rotation), 0, cos(sprite.rotation));
    float ndot = dot(n, sprite.position - ray.origin);
	t = ndot / dot(n, ray.direction);

	if(ndot > 0) n = -n;

    if(t < 0)
        return false;

	vec3 intersection_point = ray.origin + t * ray.direction;

    vec3 diff = intersection_point - sprite.position;
	float x_proj = dot(cross(n, vec3(0, 1, 0)), diff);
	float y_proj = dot(vec3(0, 1, 0), diff);

	if (
        x_proj < sprite.size.x - sprite.origin.x && 
        x_proj > -sprite.origin.x && 
        y_proj < sprite.size.y - sprite.origin.y && 
        y_proj > -sprite.origin.y
    ) {
        o_norm = n;
		return true;
	}
    return false;
}

void ray_traversal(Ray ray, inout bool o_intersected, inout float o_t, inout vec3 o_norm, inout int o_iterations) {
    o_t = 1.0 / 0.0;
    o_norm = vec3(0);
    o_intersected = false;
    o_iterations = 0;

    int maxIterations = 128;
    float d = 1;
    float current_t = 0;

    int node_it = 0;

    float t;
    if(!intersect(ray, nodes[0].bounds, t))
        return;

    if(!contains(nodes[0].bounds, ray.origin)){
        ray.origin += (t + d) * ray.direction;
        ray.length -= t + d;
        current_t += t + d;
    }

    int deep = 0;
    int deep_states[8];
    for(int i = 0; i < 8; ++i) 
        deep_states[i] = -1;

    while(maxIterations > 0) {
        ++o_iterations;
        --maxIterations;
        if(ray.length <= 0)
            break;
        Node node = nodes[node_it];

        if(deep_states[deep] != node_it){
            vec3 curr_norm;
            for(
                int child_it = nodes[node_it].child_first; 
                child_it != -1; 
                child_it = childs[child_it].child_next
            ) {
                Child child = childs[child_it];
                if(child.object_type == 0){
                    Sphere sphere;
                    sphere.position.x   = intBitsToFloat(child.data[0]);
                    sphere.position.y   = intBitsToFloat(child.data[1]);
                    sphere.position.z   = intBitsToFloat(child.data[2]);
                    sphere.radius       = intBitsToFloat(child.data[3]);

                    if(intersect_sphere(ray, sphere, t, curr_norm)){
                        o_intersected = true;
                        if(o_t > current_t + t){
                            o_t = current_t + t;
                            o_norm = curr_norm;
                        }
                    }
                } else if(child.object_type == 1) {
                    Plane plane;
                    plane.position.x    = intBitsToFloat(child.data[0]);
                    plane.position.y    = intBitsToFloat(child.data[1]);
                    plane.position.z    = intBitsToFloat(child.data[2]);

                    plane.origin.x      = intBitsToFloat(child.data[3]);
                    plane.origin.y      = intBitsToFloat(child.data[4]);
                    plane.origin.z      = intBitsToFloat(child.data[5]);

                    plane.size.x        = intBitsToFloat(child.data[6]);
                    plane.size.y        = intBitsToFloat(child.data[7]);

                    plane.rotation      = intBitsToFloat(child.data[8]);

                    if(intersect_plane(ray, plane, t, curr_norm)){
                        o_intersected = true;
                        if(o_t > current_t + t){
                            o_t = current_t + t;
                            o_norm = curr_norm;
                        }
                    }
                } else if(child.object_type == 2) {
                    Sprite sprite;
                    sprite.position.x    = intBitsToFloat(child.data[0]);
                    sprite.position.y    = intBitsToFloat(child.data[1]);
                    sprite.position.z    = intBitsToFloat(child.data[2]);

                    sprite.origin.x      = intBitsToFloat(child.data[3]);
                    sprite.origin.y      = intBitsToFloat(child.data[4]);
                    sprite.origin.z      = intBitsToFloat(child.data[5]);

                    sprite.size.x        = intBitsToFloat(child.data[6]);
                    sprite.size.y        = intBitsToFloat(child.data[7]);

                    sprite.rotation      = intBitsToFloat(child.data[8]);

                    if(intersect_sprite(ray, sprite, t, curr_norm)){
                        o_intersected = true;
                        if(o_t > current_t + t){
                            o_t = current_t + t;
                            o_norm = curr_norm;
                        }
                    }
                }
            }
            deep_states[deep] = node_it;
        }

        if(!contains(node.bounds, ray.origin)){
            if(node_it == 0)
                break;
            node_it = get_pool_position_up(node_it);
            --deep;
            d *= 2;
            continue;
        }

        // go deep
        if(node.is_devided){
            node_it = get_down_index(node_it, ray.origin) + get_pool_position(node_it);
            ++deep;
            d /= 2;
        } else{
            vec3 a = (node.bounds.a - ray.origin) / ray.direction;
            vec3 b = (node.bounds.b - ray.origin) / ray.direction;

            float tmax = min(
                min(
                    max(a.x, b.x),
                    max(a.y, b.y)
                ),  max(a.z, b.z)
            );

            ray.origin += (tmax + d) * ray.direction;
            ray.length -= (tmax + d);
            current_t += tmax + d;
        }
    }

}

uint wang_hash(inout uint seed) {
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);
	return seed;
}

float RandomFloat01(inout uint state) {
	return float(wang_hash(state)) / 4294967296.0;
}

vec3 RandomUnitVector(inout uint state) {
	float z = RandomFloat01(state) * 2.0f - 1.0f;
	float a = RandomFloat01(state) * TWO_PI;
	float r = sqrt(1.0f - z * z);
	float x = r * cos(a);
	float y = r * sin(a);
	return vec3(x, y, z);
}

vec3 ray_diffuse(inout vec3 ray_dir, vec3 norm, inout uint seed) {
	return normalize(norm * 1.001f + RandomUnitVector(seed));
}

vec3 ray_reflect(inout vec3 ray_dir, vec3 norm) {
	return normalize(ray_dir - 2 * norm * dot(ray_dir, norm));
}

void color_compute(Ray ray, inout vec4 o_color){
    int max_iterations = 8;
    int samples = 8;

    o_color = vec4(0);
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    int heat_map = 0;

    for(int s = 0; s < samples; ++s){
        uint seed = uint(uint(pixelPos.x) * uint(1973) + uint(pixelPos.y) * uint(9277) + uint(s) * uint(26699)) | uint(1);
        vec4 curr_color = vec4(1);
        Ray curr_ray = ray;

        for(int i = 0; i < max_iterations; ++i){
            bool intersected;
            float t;
            vec3 norm;
            int iterations;

            ray_traversal(curr_ray, intersected, t, norm, iterations);
            heat_map += iterations;

            if(!intersected) break;

            curr_ray.origin += t * curr_ray.direction + 0.01 * norm;
            curr_ray.direction = ray_diffuse(curr_ray.direction, norm, seed);
            curr_ray.length -= t;

            curr_color *= 0.75;
        }
        o_color += curr_color;
    }
    //o_color = vec4(heat_map/float(samples)/255.f, 0, 0, 1);
    o_color /= samples;
    o_color = sqrt(o_color);
}

uniform Camera cam;

void main() {
    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 screen_size = imageSize(colorbuffer);

    if (pixelPos.x >= screen_size.x || pixelPos.y >= screen_size.y)
        return;

    Ray r;
    float pix = (tan(cam.fov / 2.f) * cam.dist * 2) / float(screen_size.x);

    r.origin = cam.cameraPos;
	r.direction = normalize(
        cam.cameraFront * cam.dist
		+ vec3((pixelPos.x - screen_size.x / 2) * pix) * cam.cameraRight
		- vec3((pixelPos.y - screen_size.y / 2) * pix) * cam.cameraUp
    );
    r.length = 100000;

    vec4 o_col;
    color_compute(r, o_col);

    imageStore(colorbuffer, pixelPos, vec4(o_col));
}