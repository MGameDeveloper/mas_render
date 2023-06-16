#pragma once

#include "ints.h"
#include "keys.h"

struct mas_cmd;

class mas_input
{
private:
	struct mas_key
	{
		ekey      code;
		ekeystate state;
		ekeymod   mods;
	};

	struct mas_key_detail
	{
		mas_key_detail* next;
		mas_key         key;
	};

	struct mas_action_key
	{
		mas_action_key* next;
		const char* msg;
		mas_key         key;
	};

	struct mas_axis_key
	{
		mas_axis_key* next;
		const char* msg;
		float        scale;
		struct
		{
			ekey         code;
			ekeymod      mods;
		}key;
	};

	struct mas_key_event_queue
	{
		mas_key key[64];
		i32    idx = 0;
		i32    count = 64;
	};

	mas_key_event_queue event_queue;
	mas_key_detail* key_detail_list = NULL;
	mas_action_key* action_list = NULL;
	mas_axis_key* axis_list = NULL;
	mas_cmd* cmds = NULL;


public:
	mas_input(mas_cmd* input_cmds);
	~mas_input();

	mas_input() = default;
	mas_input(const mas_input&) = delete;
	mas_input(const mas_input&&) = delete;
	mas_input& operator=(const mas_input&) = delete;
	mas_input& operator=(const mas_input&&) = delete;

	static void show_memory_consumtion();

	void bind_action(ekey key, ekeystate state, ekeymod mods, const char* msg);
	void bind_axis(ekey key, ekeymod mods, const char* msg, float scale);

	void set_cmds(mas_cmd* input_cmds);
	void process();

	void on_key(ekey key, ekeystate state, ekeymod mods);
};