#pragma once
#include "../core/core.h"
#include "Texture.h"

#include <unordered_map>
#include <string_view>

//You can bind the texture by path the name and then you can reques that texture*
struct TextureManager {
	void load_texture(const char* path, const char* name);
	void load_texture_by_data(char* data, TextureParameters params, const char* name);
	Texture* get_texture(const char* name);
	~TextureManager();
private:
	std::unordered_map<std::string_view, Texture*> textures_path;
	std::unordered_map<std::string_view, Texture*> textures_names;
	air_texture_id _last_id = 0;
};