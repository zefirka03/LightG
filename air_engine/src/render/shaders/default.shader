~~vertex~~
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 camera_view;
uniform mat4 camera_proj;

out vec2 o_texCoord;
out vec4 eyeSpacePosition;

void main() {
	gl_Position = camera_proj * camera_view * vec4(position, 1.0); 
    o_texCoord = texCoord;
	eyeSpacePosition = camera_view * vec4(position, 1.0);
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

uniform sampler2D textureSamp;

in vec2 o_texCoord;
in vec4 eyeSpacePosition;

out vec4 out_color;

void main() {
	vec4 col = texture(textureSamp, vec2(1 - o_texCoord.x, 1 - o_texCoord.y));
	if(col.a < 0.01)
		discard;
	else
		out_color = col;

	if(fogParams.isEnabled) {
		float fogCoordinate = length(eyeSpacePosition);
		out_color = mix(out_color, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate / 50000));
	}
}