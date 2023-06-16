#include <glad/glad.h>
#include "mas/render.h"
#include "mas/camera.h"
#include "mas/shader.h"
#include "mas/mem_tracker.h"

static mas_mem_tracker mem("[ MAS_RENDER ]");


mas_render::mas_render(u32 quad_count)
{
	GLenum render_type = GL_DYNAMIC_DRAW;

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad) * quad_count, NULL, render_type);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vrtx_fmt), (void*)offsetof(vrtx_fmt, color));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vrtx_fmt), (void*)offsetof(vrtx_fmt, pos));

	// sprite indecies
	u32 index_size = sizeof(u32) * quad_count * 6;
	u32* indecies = (u32*)mem.alloc(index_size);
	if (!indecies)
	{
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
		mas_log::error("[ bc_render( %d ) ]: failed to allocate index buffer", quad_count);
		return;
	}
	else
	{
		mem.zero(indecies, index_size);

		u32 offset = 0;
		for (u32 idx = 0; idx < quad_count * 6; idx += 6)
		{
			indecies[0 + idx] = 0 + offset;
			indecies[1 + idx] = 1 + offset;
			indecies[2 + idx] = 2 + offset;
			indecies[3 + idx] = 0 + offset;
			indecies[4 + idx] = 3 + offset;
			indecies[5 + idx] = 2 + offset;

			offset += 4;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indecies, render_type);

		mem.dealloc(indecies);
	}

	glBindVertexArray(0);

	m_quads = (quad*)mem.alloc(sizeof(quad) * quad_count);
	count   = quad_count;
	idx     = 0;
}

mas_render::~mas_render()
{
	count = 0;
	idx = 0;
	mem.dealloc(m_quads);

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void mas_render::clear(const mas_color& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void mas_render::draw_rect(const glm::vec2& pos, const glm::vec2& size, const mas_color& color)
{
	if (idx < count)
	{
		m_quads[idx].set(pos, size, color);
		idx++;
	}
	else
	{
		mas_log::warning("[ mas_render::draw_rect ]: out of memory");
	}
}

void mas_render::begin(mas_camera* camera, mas_shader* shader)
{
	if (!shader || !camera)
		return;

	shader->bind();
	shader->set_mat4("view", camera->get_view_matrix());
	shader->set_mat4("proj", camera->get_proj_matrix());

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void mas_render::end()
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad) * idx, m_quads);
	glDrawElements(GL_TRIANGLES, idx * 4 * 6, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);

	mem.zero(m_quads, sizeof(quad) * idx);
	idx = 0;
}

bool mas_render::is_full()
{
	return (idx >= count) ? true : false;
}