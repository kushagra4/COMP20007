#include <stdio.h>
#include <stdlib.h>
#include "list.h"


List *new_queue(void)
{
	List *q;
	q=new_list();
	return q;
}

void free_queue(List *q)
{
	free_list(q);
}

void enqueue(List *q,int data)
{
	list_add_end(q,data);
}

int dequeue(List *q)
{
	return list_remove_start(q);
}

int queue_size(List *q)
{
	return list_size(q);
}
