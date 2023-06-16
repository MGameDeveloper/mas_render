#pragma once

#include <glm/glm.hpp>
#include "mas/ints.h"

class mas_camera;
class mas_shader;

class mas_color
{
public:
	union
	{
		glm::vec4 color;
		struct
		{
			float r, g, b, a;
		};
	};

public:
	mas_color(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 0)
	{
		static float f = 1.f / 255.f;
		color.r = r * f;
		color.g = g * f;
		color.b = b * f;
		color.a = a * f;
	}

	static mas_color rand_color()
	{
		static float f = 1.f / 255.f;

		mas_color c;
		c.r = (rand() % 255) * f;
		c.g = (rand() % 255) * f;
		c.b = (rand() % 255) * f;
		c.a = (rand() % 255) * f;
		return c;
	}
};


class mas_render
{
private:
	struct vrtx_fmt
	{
		glm::vec4 color;
		glm::vec2 pos;
	};

	struct quad
	{
		vrtx_fmt quad[4];

		void set(const glm::vec2& pos, const glm::vec2& size, const mas_color& color)
		{
			quad[0].pos.x = pos.x;
			quad[0].pos.y = pos.y;
			quad[1].pos.x = pos.x + size.x;
			quad[1].pos.y = pos.y;
			quad[2].pos.x = pos.x + size.x;
			quad[2].pos.y = pos.y + size.y;
			quad[3].pos.x = pos.x;
			quad[3].pos.y = pos.y + size.y;

			quad[0].color = color.color;
			quad[1].color = color.color;
			quad[2].color = color.color;
			quad[3].color = color.color;
		}
	};

	quad *m_quads = NULL;
	u32   count   = 0;
	u32   idx     = 0;

	u32 m_vao = 0;
	u32 m_vbo = 0;
	u32 m_ebo = 0;

public:
	mas_render()                              = delete;
	mas_render(const mas_render&)             = delete;
	mas_render(const mas_render&&)            = delete;
	mas_render& operator=(const mas_render&)  = delete;
	mas_render& operator=(const mas_render&&) = delete;
	
	mas_render(u32 quad_count);
	~mas_render();
	
	void clear(const mas_color& color = mas_color(0, 0, 0, 0));

	void draw_rect(const glm::vec2& pos, const glm::vec2& size, const mas_color& color);
	void begin(mas_camera* camera, mas_shader* shader);
	void end();

	bool is_full();
};