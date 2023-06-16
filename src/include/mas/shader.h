#pragma once

#include "ints.h"

struct mas_shader
{
	u32 id = 0;

	mas_shader()                              = default;
	mas_shader(const mas_shader&)             = delete;
	mas_shader(const mas_shader&&)            = delete;
	mas_shader& operator=(const mas_shader&)  = delete;
	mas_shader& operator=(const mas_shader&&) = delete;
	
	mas_shader(const char* vrtx_path, const char* frag_path);
	void make(const char* vrtx_source, const char* frag_source);
	~mas_shader();

	void create(const char* vrtx_path, const char* frag_path);
	void destroy();

	void set_bool  (const char* var_name, bool value);
	void set_int   (const char* var_name, int value);
	void set_float (const char* var_name, float value);
	void set_4float(const char* var_name, float* values);
	void set_mat4  (const char* var_name, float* mat4);
	i32  get_uniform_location(const char* uniform_name);

	void bind();
};