#version 430

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D colorbuffer;

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

bool intersect_sphere(Ray ray, Sphere sphere, inout float t){
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
        t = t0;
        return true;
    }
    return false;
}

void rayTraversal(Ray ray, inout vec4 o_color) {
    o_color = vec4(0);
    int maxIterations = 64;
    const float d = 0.005;

    int node_it = 0;

    float t;
    if(!intersect(ray, nodes[0].bounds, t))
        return;

    if(!contains(nodes[0].bounds, ray.origin)){
        ray.origin += (t + d) * ray.direction;
        ray.length -= t + d;
    }

    int deep = 0;
    int deep_states[8];
    for(int i=0; i<8; ++i) deep_states[i] = -1;

    while(maxIterations > 0) {
        maxIterations--;
        if(ray.length <= 0)
            break;

        Node node = nodes[node_it];

        if(deep_states[deep] != node_it){
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

                    if(intersect_sphere(ray, sphere, t)){
                        o_color = vec4(1,0,0,1);
                        return;
                    }
                }
            }
            deep_states[node_it] = node_it;
        }

        int ppos_up = get_pool_position_up(node_it);
        if(deep > 0 && deep_states[deep - 1] != ppos_up){
            if(node_it <= 0)
                break;
            --deep;
            node_it = ppos_up;
            continue;
        }

        // go deep
        if(node.is_devided){
            node_it = get_down_index(node_it, ray.origin) + get_pool_position(node_it);
            ++deep;
        }
        else{
            vec3 a = (node.bounds.a - ray.origin) / ray.direction;
            vec3 b = (node.bounds.b - ray.origin) / ray.direction;

            float tmax = min(
                min(
                    max(a.x, b.x),
                    max(a.y, b.y)
                ),  max(a.z, b.z)
            );

            ray.origin += (tmax + d) * ray.direction;
            ray.length -= tmax + d;
        }
    }
    //o_color = vec4(0.1 * max_depth, 0.1 * max_depth, 0.1 * max_depth, 1);
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
    r.length = 200000;

    vec4 ncol;

    rayTraversal(r, ncol);
    imageStore(colorbuffer, pixelPos, ncol);
    //float t;
    //if(intersect(r, nodes[0].bounds, t))
    //    imageStore(colorbuffer, pixelPos, vec4(1));
    //else imageStore(colorbuffer, pixelPos, vec4(0));
}