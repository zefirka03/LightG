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

envField get_curr_field(vec3 position){
  vec2 part = vec2(
    (position.x - world_origin.x) / (world_size / float(size)) - 0.5,
    (position.z - world_origin.y) / (world_size / float(size)) - 0.5
  );

  const vec2 u_l = vec2(floor(part.x), ceil(part.y));
  const vec2 u_r = vec2(ceil(part.x), ceil(part.y));
  const vec2 d_r = vec2(ceil(part.x), floor(part.y));
  const vec2 d_l = vec2(floor(part.x), floor(part.y));

  envField up_l_F = get_field(int(u_l.x), int(u_l.y));
  envField up_r_F = get_field(int(u_r.x), int(u_r.y));
  envField down_r_F = get_field(int(d_r.x), int(d_r.y));
  envField down_l_F = get_field(int(d_l.x), int(d_l.y));

  const vec2 u_l_t = length(part - u_l);
  const vec2 u_r_t = length(part - u_r);
  const vec2 d_r_t = length(part - d_r);
  const vec2 d_l_t = length(part - d_l);

  float D = u_l_t + u_r_t + d_r_t + d_l_t;

  envField outF;
  outF.v = down_l_F.v * (1 - ) + down_r_F.v * d_l_t / D + up_l_F.v * length(part - u_l) + up_r_F.v * (1 - length(part - u_r)); 

  return outF;
}

vec3 grassPosition = grassPositions[gl_InstanceID].position;
envField curr_filed = get_curr_field(grassPosition);
const float windSpeed = 0.1;
const float windDisplacement = 0.02;

uniform float time;
uniform mat4 camera_view;
uniform mat4 camera_proj;

out float height;
out vec4 eyeSpacePosition;

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
		windDirection = normalize(windDirection) * 5 * sigmoid(5 * len);
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

	gl_Position = camera_proj * camera_view * vec4(finalPosition, 1.0);
	eyeSpacePosition = camera_view * vec4(finalPosition, 1.0);
}

~~fragment~~
#version 430 core

struct FogParameters {
  vec3 color;
  float linearStart;
  float linearEnd;
  float density;

  int equation;
  bool isEnabled;
};

float getFogFactor(FogParameters params, float fogCoordinate) {
  float result = 0.0;
  if(params.equation == 0) {
    float fogLength = params.linearEnd - params.linearStart;
    result = (params.linearEnd - fogCoordinate) / fogLength;
  }
  else if(params.equation == 1) {
    result = exp(-params.density * fogCoordinate);
  }
  else if(params.equation == 2) {
    result = exp(-pow(params.density * fogCoordinate, 2.0));
  }

  result = 1.0 - clamp(result, 0.0, 1.0);
  return result;
}

const FogParameters fogParams = {
  vec3(1),
  7.0,
  100000.0,
  0.7,
  1,
  true
};

out vec4 FragColor;
in float height;
in vec4 eyeSpacePosition;

void main() {
	FragColor = vec4(mix(vec3(0, 0.2, 0), vec3(0.4, 0.9, 0.2), vec3(height)), 1);
	if(fogParams.isEnabled) {
		float fogCoordinate = length(eyeSpacePosition);
		FragColor = mix(FragColor, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate / 50000));
	}
}