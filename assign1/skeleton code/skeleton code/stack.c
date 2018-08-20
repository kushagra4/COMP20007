#include <stdio.h>
#include <stdlib.h>
#include "list.h"


List *new_stack(void)
{
	List *s;
	s=new_list();
	return s;
}
	
void free_stack(List *s)
{
	free_list(s);
}

void stack_push(List *s,int data)
{
	list_add_start(s,data);
}

int stack_pop(List *s)
{
	return list_remove_start(s);
}

int stack_size(List *s)
{
	return list_size(s);
}
	
