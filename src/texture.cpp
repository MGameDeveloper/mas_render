#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <string.h>
#include <glad/glad.h>
#include "mas/texture.h"
#include "mas/log.h"
#include "mas/shader.h"

namespace pool
{
	struct mas_texture_data
	{
		const char* path = NULL;
		u32         id = 0;
		i32         bound_idx = 0;
		i32         use_count = 0;
		i32         width = 0;
		i32         height = 0;
		float       width_norm = 0.0f;
		float       height_norm = 0.0f;
	};

	static mas_texture_data textures[32];
	static i32             idx = 0;
	static i32             count = 32;
	static bool            should_rebind = false;
	static bool            has_dumb_texture = false;

	static void setup_dumb_texture()
	{
		mas_texture_data* texture = &textures[idx++];

		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_2D, texture->id);

		u8 data[]{ 255, 255, 255, 255 };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		texture->use_count += 1;
		texture->path = "mas_dumb_texture";
		texture->width = 1;
		texture->height = 1;
		texture->width_norm = 1.f / float(1);
		texture->height_norm = 1.f / float(1);
		should_rebind = true;

		has_dumb_texture = true;
	}

	static i32 load(const char* path)
	{
		if (!has_dumb_texture)
		{
			mas_log::error("[ mas_texture ]: you must call mas_texture::init to initialize it");
			return -1;
		}

		mas_texture_data* texture = NULL;

		for (i32 i = 0; i < idx; ++i)
		{
			if (strcmp(textures[i].path, path) == 0)
			{
				texture = &textures[i];
				texture->use_count++;
				return texture->bound_idx;
			}
		}

		if (idx < count)
		{
			texture = &textures[idx++];
			texture->bound_idx = idx - 1;
		}
		else
		{
			for (i32 i = 0; i < count; ++i)
			{
				if (textures[i].use_count <= 0)
				{
					texture = &textures[i];
					break;
				}
			}
		}

		if (!texture)
			return -1;

		GLint fmt;
		i32 w, h, comp;
		u8* data = stbi_load(path, &w, &h, &comp, 0);
		if (!data)
		{
			stbi_image_free(data);
			mas_log::error("failed to load texture from [ %s ]", path);
			return -1;
		}
		else
		{
			if (comp == 3)
				fmt = GL_RGB;
			else if (comp == 4)
				fmt = GL_RGBA;
			else
			{
				stbi_image_free(data);
				mas_log::error("texture [ %s ] unknown format");
				return -1;
			}
		}

		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_2D, texture->id);

		glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);

		texture->use_count += 1;
		texture->path = path;
		texture->width = w;
		texture->height = h;
		texture->width_norm = 1.f / float(w);
		texture->height_norm = 1.f / float(h);
		should_rebind = true;

		return texture->bound_idx;
	}

	static void unload(mas_texture* texture)
	{
		if (!texture)
			return;

		mas_texture_data* t = &textures[texture->idx_into_pool()];

		t->use_count--;
		if (t->use_count <= 0)
		{
			glDeleteTextures(1, &t->id);

			t->path = NULL;
			t->width = 0;
			t->height = 0;
			t->width_norm = 0.f;
			t->height_norm = 0.f;
		}
	}
};

mas_texture::mas_texture(const char* path)
{
	pool_idx = pool::load(path);
	if (pool_idx == -1)
		mas_log::error("[ mas_texture( %s ) ]: failed", path);
}

mas_texture::~mas_texture()
{
	pool::unload(this);
	pool_idx = -1;
}

glm::vec2 mas_texture::size()
{
	if (pool_idx == -1)
		return glm::vec2(0.f);

	return glm::vec2(pool::textures[pool_idx].width, pool::textures[pool_idx].height);
}

glm::vec2 mas_texture::size_norm()
{
	if (pool_idx == -1)
		return glm::vec2(0.f);

	return glm::vec2(pool::textures[pool_idx].width_norm, pool::textures[pool_idx].height_norm);
}

i32 mas_texture::idx()
{
	if (pool_idx == -1)
		return 0;

	return pool::textures[pool_idx].bound_idx;
}

i32 mas_texture::use_count()
{
	if (pool_idx == -1)
		return 0;

	return pool::textures[pool_idx].use_count;
}

const char* mas_texture::path()
{
	if (pool_idx == -1)
		return 0;

	return pool::textures[pool_idx].path;
}

i32 mas_texture::idx_into_pool()
{
	if (pool_idx == -1)
		return 0;

	return pool_idx;
}

void mas_texture::bind(mas_shader* shader)
{
	if (!shader)
		return;

	static char texture_element[256];

	if (pool::should_rebind)
	{
		for (i32 idx = 0; idx < pool::count; ++idx)
		{
			if (pool::textures[idx].use_count > 0)
			{
				glActiveTexture(GL_TEXTURE0 + idx);
				glBindTexture(GL_TEXTURE_2D, pool::textures[idx].id);

				memset(texture_element, 0, 256);
				sprintf_s(texture_element, "textures[%d]", idx);

				shader->set_int(texture_element, pool::textures[idx].bound_idx);

				mas_log::trace("[ mas_texture::bind ]: binding %s at %d", pool::textures[idx].path, idx);
				mas_log::trace("[ mas_texture::bind ]: %s = %d", texture_element, pool::textures[idx].id);
			}
		}

		pool::should_rebind = false;
	}
}

void mas_texture::init()
{
	pool::setup_dumb_texture();
}

mas_texture::mas_texture(mas_texture* font, mas_texture* tex, u32 font_size)
{
	if (!font || !tex)
		return;


	static const char* vrtx_shader =
		"#version 460 core\n"
		"layout (location = 0) in vec2 POS;\n"
		"layout (location = 1) in vec2 UV;\n"

		"out vec2 uv;\n"

		"void main()\n"
		"{\n"
		"gl_Position = vec4(POS.x, POS.y, 0.f, 1.f);\n"
		"uv = UV;\n"
		"}";

	static const char* frag_shader =
		"#version 460 core\n"

		"out vec4 frag_color;\n"

		"in vec2 uv;\n"
		"uniform sampler2D font;\n"
		"uniform sampler2D tex;\n"
		
		"void main()\n"
		"{\n"
		"frag_color = vec4(1.f, 0.f, 0.f, 1.f);\n"
		"}";

	static mas_shader shader;
	static bool has_shader = false;
	if (!has_shader)
	{
		shader.make(vrtx_shader, frag_shader);
		has_shader = true;
	}


	GLuint framebuffer_id = 0;
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	// CREATE TEXTURE
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// SET TEXTURE PARAMS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->size().x * font_size, font->size().y * font_size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ATTACK TEXTURE AS COLRO BUFFER FOR THE FRAME BUFFER
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		mas_log::error("[ make_texture ]: render framebuffer is not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &framebuffer_id);
		glDeleteTextures(1, &texture_id);
		return;
	}


	struct vrtx_fmt
	{
		glm::vec2 pos;
		glm::vec2 uv;
	};
	static vrtx_fmt vs[4];
	static bool has_vs = false;
	if (!has_vs)
	{
		vs[0].pos = { 0.f, 0.f };
		vs[1].pos = { 1.f, 0.f };
		vs[2].pos = { 1.f, 1.f };
		vs[3].pos = { 0.f, 1.f };

		vs[0].uv = { 0.f, 0.f };
		vs[1].uv = { 1.f, 0.f };
		vs[2].uv = { 1.f, 1.f };
		vs[3].uv = { 0.f, 1.f };

		has_vs = true;
	}

	u32 vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	u32 buffer_size = ( font->size().x * font->size().y * font_size );
	glBufferData(GL_ARRAY_BUFFER, sizeof(vrtx_fmt) * 4, vs, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vrtx_fmt), (void*)offsetof(vrtx_fmt, pos));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vrtx_fmt), (void*)offsetof(vrtx_fmt, uv));
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0 + font->idx());
	glActiveTexture(GL_TEXTURE0 + tex->idx());

	shader.bind();
	shader.set_int("font", font->idx());
	shader.set_int("tex", tex->idx());

	glDrawArrays(GL_TRIANGLES, 0, 1);
	//glDrawBuffer(1);

	// Back to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glActiveTexture(GL_TEXTURE0);
	shader.set_int("result_sampler", texture_id);

	glDrawArrays(GL_TRIANGLES, 0, 1);
}