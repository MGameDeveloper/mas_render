#pragma once

#include <glm/glm.hpp>

class mas_camera
{
private:
	glm::mat4 m_proj      = glm::mat4(1.f);
	glm::mat4 m_view      = glm::mat4(1.f);

public:
	mas_camera()                              = default;
	~mas_camera()                             = default;
	mas_camera(const mas_camera&)             = delete;
	mas_camera(const mas_camera&&)            = delete;
	mas_camera& operator=(const mas_camera&)  = delete;
	mas_camera& operator=(const mas_camera&&) = delete;

	mas_camera(float view_width, float view_height, float near_plane, float far_plane);
	float* get_view_matrix();
	float* get_proj_matrix();
};