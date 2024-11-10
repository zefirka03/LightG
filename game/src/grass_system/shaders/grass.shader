~~vertex~~
#version 430 core

struct grass{
    vec3 position;
    float _padding;
};

layout (location = 0) in vec3 position;

layout (std430, binding = 0) buffer positionsBuffer {
  grass grassPositions[];
};

const vec2 windDirection = normalize(vec2(0.2, 0.5));
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
  return abs(sin((grassPosition.x * windDirection.x + grassPosition.y * windDirection.y) + windSpeed * time))*1.3 + (sin(time * 10 + rand(grassPosition) * 40) * 0.03);
}

void main() {
      vec3 grassPosition = grassPositions[gl_InstanceID].position;

      float random = rand(grassPosition.xz);

      float localWindVariance = random * 0.5;
      float rotation = rand(grassPosition.xz + vec2(5)) * 360.0;

      vec3 localPos = (rotationY(rotation) * vec4(position * 10000, 1.0)).xyz;
      vec3 finalPosition = vec3(localPos.x + grassPosition.x, localPos.y + grassPosition.y, localPos.z + grassPosition.z);

      height = position.y / 0.06;

      vec2 displacement = 10000 * getDisplacementMap(grassPosition.xz) * windDirection;
      finalPosition += vec3(displacement.x + localWindVariance, 0, displacement.y + localWindVariance) * (height*height) * windDisplacement;

      gl_Position = camera * vec4(finalPosition, 1.0);
}

~~fragment~~
#version 430 core

out vec4 FragColor;
in float height;


void main() {
  FragColor = vec4(mix(vec3(0, 0.2, 0), vec3(0.4, 0.9, 0.2), vec3(height)), 1);
  
}