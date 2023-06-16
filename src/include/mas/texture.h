#pragma once

#include <glm/glm.hpp>
#include "ints.h"

class mas_shader;

class mas_texture
{
private:
	i32 pool_idx = 0;

public:
	mas_texture(const char* path);
	~mas_texture();

	mas_texture(mas_texture* font, mas_texture* tex, u32 font_size);
	mas_texture()                               = default;
	mas_texture(const mas_texture&)             = delete;
	mas_texture(const mas_texture&& other)      = delete;
	mas_texture& operator=(const mas_texture&)  = delete;
	mas_texture& operator=(const mas_texture&&) = delete;

	glm::vec2   size();
	glm::vec2   size_norm();
	i32         idx();
	i32         use_count();
	const char* path();
	i32         idx_into_pool();

	static void bind(mas_shader* shader);
	static void init();
};