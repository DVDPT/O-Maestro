#pragma once

#include "lists.h"

typedef struct _bounded_queue
{
	short * arr;
	int cap;
	int get;
	int put;
	dlist_node node;
}bounded_queue;

void bounded_queue_init(bounded_queue * queue);
void bounded_queue_get

