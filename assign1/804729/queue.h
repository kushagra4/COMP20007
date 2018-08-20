#include <stdio.h>
#include <stdlib.h>
#include "list.h"


List *new_queue(void);

	
void free_queue(List *q);


void enqueue(List *q,int data);

int dequeue(List *q);


int queue_size(List *q);

	