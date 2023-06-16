#include <glm/gtc/matrix_transform.hpp>
#include "mas/camera.h"

mas_camera::mas_camera(float view_width, float view_height, float near_plane, float far_plane)
{
	m_proj = glm::ortho(0.f, view_width, view_height, 0.f, near_plane, far_plane);
	m_view = glm::lookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
}

float* mas_camera::get_view_matrix()
{
	return &m_view[0][0];
}

float* mas_camera::get_proj_matrix()
{
	return &m_proj[0][0];
}