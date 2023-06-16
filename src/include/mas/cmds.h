#pragma once

#include "ints.h"

typedef void(*action_callback_t)(void);
typedef void(*axis_callback_t)(float);

class mas_cmd
{
private:
	enum class cmd_e { action, axis };

	struct cmd
	{
		cmd* next;

		union
		{
			action_callback_t action = NULL;
			axis_callback_t   axis;
		};

		const char* msg;
		cmd_e       type;
	};

	cmd* list = NULL;

public:
	mas_cmd() = default;
	mas_cmd(const mas_cmd&) = delete;
	mas_cmd(const mas_cmd&&) = delete;
	mas_cmd& operator=(const mas_cmd&) = delete;
	mas_cmd& operator=(const mas_cmd&&) = delete;
	~mas_cmd();

	void add_action(const char* msg, action_callback_t action);
	void replace_action(const char* msg, action_callback_t action);

	void add_axis(const char* msg, axis_callback_t axis);
	void replace_axis(const char* msg, axis_callback_t axis);

	void call(const char* msg, float scale = 0);

	static void show_memory_consumtion();
};