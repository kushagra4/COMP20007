#include <stdio.h>
#include <stdlib.h>
#include "list.h"


List *new_stack(void);

	
void free_stack(List *s);


void stack_push(List *s,int data);

int stack_pop(List *s);


int stack_size(List *s);

	