#pragma once

struct mas_time
{
	double elapsed_time = 0.0f;
	double start_time = 0.0f;
	double dt = 0.0f;

	mas_time();
	~mas_time() = default;
	mas_time(const mas_time&) = delete;
	mas_time(const mas_time&&) = delete;
	mas_time& operator=(const mas_time&) = delete;
	mas_time& operator=(const mas_time&&) = delete;

	void calculate_dt();
};