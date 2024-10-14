#pragma once
#include "../core/core.h"

#define AIR_TEXTURE_LINEAR GL_LINEAR
#define AIR_TEXTURE_NEAREST GL_NEAREST
#define AIR_TEXTURE_RGB GL_RGB
#define AIR_TEXTURE_RGBA GL_RGBA
#define AIR_TEXTURE_R8 GL_R8
#define AIR_TEXTURE_RED GL_RED

typedef GLuint		air_texture_id;

struct TextureParameters {
	int size_x = 100;
	int size_y = 100;
	uint16_t internalFormat = AIR_TEXTURE_RGBA;
	uint16_t format = AIR_TEXTURE_RGBA;
};

struct Texture {
	air_texture_id id = 0;
};