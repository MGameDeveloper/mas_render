#pragma once

#include "ints.h"
#include <glm/glm.hpp>

class mas_shader;

struct mas_clut
{
	glm::vec4* colors;
	i32        clut_count;
	i32        color_count_per_clut;

	mas_clut() = default;
	mas_clut(const mas_clut&) = delete;
	mas_clut(const mas_clut&&) = delete;
	mas_clut& operator=(const mas_clut&) = delete;
	mas_clut& operator=(const mas_clut&&) = delete;

	mas_clut(i32 clut_count, i32 color_per_clut);
	~mas_clut();

	void      create(i32 clut_count, i32 color_per_clut);
	void      destroy();
	void      set_color(i32 clut_idx, i32 color_idx, const glm::vec4& color);
	glm::vec4 get_color(i32 clut_idx, i32 color_idx);

	void bind(mas_shader* shader);

	static void show_memory_consumption();
};