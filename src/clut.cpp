#include "mas/clut.h"
#include "mas/shader.h"
#include "mas/mem_tracker.h"

static mas_mem_tracker mas_mem("[ mas_CLUT ]");

mas_clut::mas_clut(i32 clut_count, i32 color_per_clut)
{
	create(clut_count, color_per_clut);
}

mas_clut::~mas_clut()
{
	destroy();
}

void mas_clut::create(i32 clut_count, i32 color_per_clut)
{
	if (clut_count <= 0 || color_per_clut <= 0)
	{
		mas_log::warning("[ mas_clut::create( %d, %d) ]: one of the params is not correct", clut_count, color_per_clut);
		return;
	}

	u32 size = (sizeof(glm::vec4) * (color_per_clut * clut_count));
	colors = (glm::vec4*)mas_mem.alloc(size);
	if (colors)
	{
		mas_mem.zero(colors, size);
		this->clut_count = clut_count;
		this->color_count_per_clut = color_per_clut;
	}
	else
	{
		mas_log::error("[ mas_clut::create( %d, %d ) ]: creation failed", clut_count, color_per_clut);
	}
}

void mas_clut::destroy()
{
	if (colors)
	{
		mas_mem.dealloc(colors);
		clut_count = 0;
		color_count_per_clut = 0;
	}
}

void mas_clut::set_color(i32 clut_idx, i32 color_idx, const glm::vec4& color)
{
	if (clut_count <= clut_idx || color_count_per_clut <= color_idx || !colors)
		return;

	static float f = 1.f / 255.f;
	colors[clut_idx * color_count_per_clut + color_idx] = (color * f);
}

glm::vec4 mas_clut::get_color(i32 clut_idx, i32 color_idx)
{
	static glm::vec4 dump_color(0.5, 0.5, 0.5, 1.f);

	if (clut_count <= clut_idx || color_count_per_clut <= color_idx || !colors)
		return dump_color;

	return colors[clut_idx * color_count_per_clut + color_idx];
}

void mas_clut::bind(mas_shader* shader)
{
	if (!shader || !colors)
		return;

	static char clut_elemnt[256];
	mas_mem.zero(clut_elemnt, 256);

	for (u32 clut_idx = 0; clut_idx < clut_count; ++clut_idx)
	{
		for (u32 color_idx = 0; color_idx < color_count_per_clut; ++color_idx)
		{
			//std::string var(clut_name + "[" + std::to_string(clut_idx) + "][" + std::to_string(color_idx) + "]");
			//mas_shader::set_4float(shader, var.c_str(), &clut->colors[clut_idx * clut->color_count_per_clut + color_idx][0]);
			mas_mem.zero(clut_elemnt, 256);
			sprintf_s(clut_elemnt, "clut[%d][%d]", clut_idx, color_idx);

			shader->set_4float(clut_elemnt, &colors[clut_idx * color_count_per_clut + color_idx][0]);
		}
	}
}

void mas_clut::show_memory_consumption()
{
	mas_mem.print_usage();
}