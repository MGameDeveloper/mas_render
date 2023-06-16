#include "mas/cmds.h"
#include "mas/mem_tracker.h"

static mas_mem_tracker mas_mem("[ mas_CMDS ]");

mas_cmd::~mas_cmd()
{
	cmd* c = list;
	for (; c; c = list)
	{
		list = c->next;
		mas_mem.dealloc(c);
	}
}

void mas_cmd::add_action(const char* msg, action_callback_t action)
{
	cmd* c = list;
	for (; c; c = c->next)
	{
		if (!c->action && !c->msg)
		{
			c->msg = msg;
			c->action = action;
			c->type = cmd_e::action;
			return;
		}
	}

	c = (cmd*)mas_mem.alloc(sizeof(cmd));
	c->msg = msg;
	c->action = action;
	c->type = cmd_e::action;
	c->next = list;
	list = c;
}

void mas_cmd::replace_action(const char* msg, action_callback_t action)
{
	cmd* c = list;
	for (; c; c = c->next)
	{
		if (strcmp(c->msg, msg) == 0)
		{
			if (c->type == cmd_e::action)
				c->action = action;
			else
				mas_log::warning("[ mas_cmd::replace_action( %s ) ]: must be action to replace", msg);

			return;
		}
	}

	mas_log::warning("[ mas_cmd::add ]: couldn't find %s cmd to replace consider mas_cmd::add instead", msg);
}

void mas_cmd::add_axis(const char* msg, axis_callback_t axis)
{
	cmd* c = list;
	for (; c; c = c->next)
	{
		if (!c->action && !c->msg)
		{
			c->msg = msg;
			c->axis = axis;
			c->type = cmd_e::axis;
			return;
		}
	}

	c = (cmd*)mas_mem.alloc(sizeof(cmd));
	c->msg = msg;
	c->axis = axis;
	c->type = cmd_e::axis;
	c->next = list;
	list = c;
}

void mas_cmd::replace_axis(const char* msg, axis_callback_t axis)
{
	cmd* c = list;
	for (; c; c = c->next)
	{
		if (strcmp(c->msg, msg) == 0)
		{
			if (c->type == cmd_e::axis)
				c->axis = axis;
			else
				mas_log::warning("[ mas_cmd::replace_axis( %s ) ]: must be axis to replace", msg);

			return;
		}
	}

	mas_log::warning("[ mas_cmd::add ]: couldn't find %s cmd to replace consider mas_cmd::add instead", msg);
}

void mas_cmd::call(const char* msg, float scale)
{
	cmd* c = list;
	for (; c; c = c->next)
	{
		if (strcmp(c->msg, msg) == 0)
			break;
	}

	if (c)
	{
		if (c->action && c->type == cmd_e::action)
		{
			c->action();
		}
		else if (c->axis && c->type == cmd_e::axis)
		{
			c->axis(scale);
		}
		else
			mas_log::warning("[ mas_cmd::call( %s ) ]: doesn't have callback bound to it", msg);
	}

}

void mas_cmd::show_memory_consumtion()
{
	mas_mem.print_usage();
}