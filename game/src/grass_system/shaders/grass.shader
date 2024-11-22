~~vertex~~
#version 430 core

struct envField {
    vec2 v;
};

struct grass{
    vec3 position;
    float _padding;
};

layout (location = 0) in vec3 position;

layout (std430, binding = 0) buffer positionsBuffer {
  grass grassPositions[];
};

layout (std430, binding = 1) buffer fieldBuffer {
  envField field[];
};

uniform int size;
uniform float world_size;
uniform vec2 world_origin;

envField get_field(int i, int j){
    envField out_f;
    out_f.v = vec2(0);
    if(i < 0 || j < 0 || i >= size || j >= size)
        return out_f;

    return field[i * size + j];
}

vec3 grassPosition = grassPositions[gl_InstanceID].position;
envField curr_filed = get_field(
	int(floor((grassPosition.x - world_origin.x) / (world_size / float(size)))), 
	int(floor((grassPosition.z - world_origin.y) / (world_size / float(size))))
);
const float windSpeed = 0.1;
const float windDisplacement = 0.02;

uniform float time;

out float height;
uniform mat4 camera;

mat4 rotationY( in float angle ) {
	return mat4(cos(angle), 0,      sin(angle),	0,
                0,		     1.0,	  0,	        0,
					        -sin(angle),	0,		  cos(angle),	0,
                0, 		      0,			0,	        1);
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float getDisplacementMap(vec2 grassPosition) {
	return abs(sin((grassPosition.x + grassPosition.y) + windSpeed * time))*1.3 + (sin(time * 10 + rand(grassPosition) * 40) * 0.03);
}

float sigmoid(float x){
	return 1.f / (1.f + exp(-x));
}

void main() {
	vec2 windDirection = vec2(curr_filed.v.x, curr_filed.v.y);

	float len = length(windDirection);
	if (len > 0.0) {
		windDirection = normalize(windDirection) * 3 * sigmoid(5 * len);
	} else {
		windDirection = vec2(0.0, 0.0);
	}

	float random = rand(grassPosition.xz);

	float localWindVariance = random * 0.5;
	float rotation = rand(grassPosition.xz + vec2(5)) * 360.0;

	vec3 localPos = (rotationY(rotation) * vec4(position * 10000, 1.0)).xyz;
	vec3 finalPosition = vec3(localPos.x + grassPosition.x, localPos.y + grassPosition.y, localPos.z + grassPosition.z);

	height = position.y / 0.06;

	vec2 displacement = 5000 * getDisplacementMap(grassPosition.xz) * windDirection;
	finalPosition += vec3(displacement.x + localWindVariance, 0, displacement.y + localWindVariance) * (height * height) * windDisplacement;

	gl_Position = camera * vec4(finalPosition, 1.0);
}

~~fragment~~
#version 430 core

out vec4 FragColor;
in float height;

void main() {
	FragColor = vec4(mix(vec3(0, 0.2, 0), vec3(0.4, 0.9, 0.2), vec3(height)), 1);
}