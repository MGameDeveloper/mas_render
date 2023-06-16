#pragma once

#include <stdlib.h>
#include <string.h>
#include "ints.h"
#include "log.h"

class mas_mem_tracker
{
private:
	struct blk_s
	{
		blk_s* next = NULL;
		void* data = NULL;
		u32    size = 0;
	};

	const char* name = NULL;
	blk_s* blks = NULL;
	u32         alloc_count = 0;
	u32         free_count = 0;
	u32         alloc_size = 0;
	u32         free_size = 0;
	u32         occupied_size = 0;

private:
	void release_blk(blk_s** b)
	{
		if (b && (*b))
		{
			free_count++;
			free_size += (*b)->size;
			alloc_size -= (*b)->size;
			occupied_size -= (*b)->size;

			free((*b)->data);
			(*b)->data = NULL;

			free((*b));
			(*b) = NULL;
		}
	}

public:
	mas_mem_tracker(const char* name) : name(name) { }
	~mas_mem_tracker()
	{
		// wont delete allocated data
		// only the trackers blks
		// call dealloc to delete the actual data
		blk_s* blk = blks;
		for (; blk;)
		{
			free_count++;
			alloc_size -= blk->size;
			free_size += blk->size;
			occupied_size -= blk->size;

			blk_s* b = blk;
			blk = blk->next;

			free(b);
			b = NULL;
		}
		blks = NULL;
	}

	void* alloc(u32 size)
	{
		void* p = malloc(size);
		if (!p)
		{
			mas_log::error("[ mas_memory_tracker_s::alloc( %s ) ]: failed to allocate", size);
			return NULL;
		}
		memset(p, 0, size);


		blk_s* blk = (blk_s*)malloc(sizeof(blk_s));
		if (!blk)
		{
			mas_log::error("[ mas_memory_tracker_s::alloc ]: failed to tracking data");
			free(p);
			return NULL;
		}

		blk->data = p;
		blk->size = size;
		blk->next = NULL;

		blk->next = blks;
		blks = blk;

		alloc_count++;
		alloc_size += size;
		occupied_size += alloc_size;

		return p;
	}

	void dealloc(void* data)
	{
		blk_s* prev_blk = NULL;
		blk_s* curr_blk = blks;

		for (; curr_blk; curr_blk = curr_blk->next)
		{
			if (curr_blk->data == data)
			{
				if (!prev_blk) // means we are at the first blk
				{
					blks = curr_blk->next;
					release_blk(&curr_blk);

					break;
				}
				else if (!curr_blk->next) // means we are at the last blk
				{
					prev_blk->next = NULL;
					release_blk(&curr_blk);

					break;
				}
				else // means we are in the middle of blks
				{
					prev_blk->next = curr_blk->next;
					release_blk(&curr_blk);

					break;
				}
			}

			prev_blk = curr_blk;
		}

	}

	void zero(void* data, u32 size)
	{
		if (data)
			memset(data, 0, size);
	}

	void print_usage()
	{
		mas_log::text("\n============================");
		mas_log::text("= BEGING: %s", name);
		mas_log::text("============================");
		mas_log::text("1- ALLOC_COUNT: %d", alloc_count);
		mas_log::text("2- FREE_COUNT : %d", free_count);
		mas_log::text("3- ALLOC_SIZE : %d", alloc_size);
		mas_log::text("4- FREE_SIZE  : %d", free_size);
		mas_log::text("5- OCCUPIED   : %d", occupied_size);
		mas_log::text("============================");
		mas_log::text("= END: %s", name);
		mas_log::text("============================\n");
	}
};

