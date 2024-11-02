#version 430

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D colorbuffer;

struct boundingBox {
    vec3 a;
	vec3 b;

	vec3 center;
	float diameter;
};

struct Child {
    int child_next;
    int child_prev;
    int node_it;
    int data[16];
};

struct Node {
    boundingBox bounds;
    int childs_size;
    int child_first;
    int child_last;
    int deep;
    int pool_position;
    bool is_devided;
};

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

layout(std430, binding = 1) buffer Childs {
    Child childs[];
};

layout(std430, binding = 1) buffer Nodes {
    Node nodes[];
};

int get_pool_position(int i) {
    return 8 * i + 1;
}

bool intersect(Ray ray, boundingBox bbox, out float t) {
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

void rayTraversal(int nodeIndex, Ray ray, out vec4 out_color) {
    Node node = nodes[nodeIndex];
    float t;

    if(intersect(ray, node.bounds, t)) {
        out_color += vec4(0.1,0.1,0.1,0.1);
        for (int i = node.child_first; i != -1; i = childs[i].child_next) {
            continue;
        }

        Ray new_ray;
        new_ray.origin = ray.origin + (t + 0.001) * ray.direction;
        new_ray.direction = ray.direction;
        new_ray.length = ray.length - t;

        if (new_ray.length < 0.0) return;

        if (node.is_devided) {
            int ppos = get_pool_position(nodeIndex);
            for (int i = 0; i < 8; ++i) {
                rayTraversal(ppos + i, new_ray, out_color);
            }
        }
    }
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
		+ vec3((pixelPos.y - screen_size.y / 2) * pix) * cam.cameraUp
    );

    vec4 ncol = vec4(cam.cameraFront, 1.0);
    
    imageStore(colorbuffer, pixelPos, ncol);
}