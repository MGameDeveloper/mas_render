#pragma once

#include <glm/glm.hpp>

class mas_transform
{
private:
	glm::vec3 m_position = glm::vec3(0.f);
	glm::vec3 m_size = glm::vec3(8.f, 8.f, 0.f);
	glm::vec3 m_rotate_axis = glm::vec3(0.f);
	glm::vec3 m_scale = glm::vec3(1.f);
	float     m_rotate_angle = 0.f;

public:
	mas_transform();
	~mas_transform();
	mas_transform(const mas_transform&) = delete;
	mas_transform(const mas_transform&&) = delete;
	mas_transform& operator=(const mas_transform&) = delete;
	mas_transform& operator=(const mas_transform&&) = delete;

	glm::vec3 get_position();
	glm::vec3 get_scale();
	glm::vec3 get_size();
	glm::vec3 get_rotate_axis();
	float     get_rotate_angle();
	void      set_size(const glm::vec3& size);

	void translate(const glm::vec3& pos);
	void rotate(const glm::vec3& axis, float angle);
	void scale(const glm::vec3& scale);

	glm::vec4 apply_tranform(const glm::vec3& vrtx_pos);
};
