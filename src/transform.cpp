#include <glm/gtc/matrix_transform.hpp>
#include "mas/transform.h"

mas_transform::mas_transform()
{

}

mas_transform::~mas_transform()
{

}

glm::vec3 mas_transform::get_position()
{
	return m_position;
}

glm::vec3 mas_transform::get_scale()
{
	return m_scale;
}

glm::vec3 mas_transform::get_size()
{
	return m_size;
}

glm::vec3 mas_transform::get_rotate_axis()
{
	return m_rotate_axis;
}

float mas_transform::get_rotate_angle()
{
	return m_rotate_angle;
}

void mas_transform::set_size(const glm::vec3& size)
{
	m_size = size;
}

void mas_transform::translate(const glm::vec3& pos)
{
	m_position = pos;
}

void mas_transform::rotate(const glm::vec3& axis, float angle)
{
	m_rotate_axis = axis;
	m_rotate_angle = angle;
}

void mas_transform::scale(const glm::vec3& scale)
{
	m_scale = scale;
}

glm::vec4 mas_transform::apply_tranform(const glm::vec3& vrtx_pos)
{
	static glm::mat4 m;
	m = glm::mat4(1.f);

	m = glm::translate(m, m_position);
	if (m_rotate_angle > 0.f)
		m = glm::rotate(m, glm::radians(m_rotate_angle), m_rotate_axis);
	m = glm::scale(m, m_scale);

	return (m * glm::vec4(vrtx_pos, 1.f));
}