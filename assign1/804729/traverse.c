/* ... */

#include <stdio.h>
#include <stdlib.h>
#include "traverse.h"
#include "list.h"
#include "stack.h"
#include "queue.h"

/****************************************************************/

void print_all_paths(Graph* graph,int source, int d, int visited[],
                              int path[], int *path_index);

void find_shortest_path(Graph* graph,int source, int d, int visited[],
                              int w, int *path_index,int *min,int path[],
                              int min_path[],int *shortest_path_index);

/****************************************************************/



void print_dfs(Graph* graph, int source_id) {
	
	/* the array 'arr' stores that a particular node was visited or not */
	
	int arr[graph->n],i,id=source_id,source;
	
	/*the for loop initialises all indexes of the array 'arr' to zero because 
	initially you want to imply that no node is visited*/
	for(i=0;i<graph->n;i++)
	{
		arr[i]=0;
	}
	List *s=new_stack();
	stack_push(s,source_id);  //pushes the first node onto the stack
	arr[id]=1;  
	
	while(stack_size(s))
	{
		id=stack_pop(s); //pops out the last node of the stack and stores its id
		printf("%s\n",graph->vertices[id]->label);
		Edge *e=graph->vertices[id]->first_edge; /* 'e' stores the address of 
		                             the adjacent edges to the node at 'id'*/
		
			while(e!=NULL)
			{
				source=e->v; 
				
				/*if the node at 'source' is not visited it is placed on
				the stack and also in array 'arr' it is marked as visited*/
				
				if(arr[source]==0)
				{
					stack_push(s,source);
					arr[source]=1;
				}
				e=e->next_edge; 
			}	
		}
	
	printf("not yet implemented: put code for part 1 here\n");
}


/****************************************************************/


void print_bfs(Graph* graph, int source_id) {
	
	/* the array 'arr' stores that a particular node was visited or not*/
	int arr[graph->n],i,id=source_id,source;
	/*the for loop initialises all indexes of the array 'arr' to zero because 
	initially you want to imply that no node is visited*/
	for(i=0;i<graph->n;i++)
	{
		arr[i]=0;
	}
	List *q=new_queue(); //pushes the first node onto the queue
	enqueue(q,source_id);
	arr[id]=1;
	
	while(queue_size(q))
	{
		id=dequeue(q); //removes the first node from the queue and stores its id
		printf("%s\n",graph->vertices[id]->label);
		Edge *e=graph->vertices[id]->first_edge; /* 'e' stores the address of 
		                             the adjacent edges to the node at 'id'*/
		
			while(e!=NULL)
			{
				source=e->v;
				
				/*if the node at 'source' is not visited it is placed on
				the stack and also in array 'arr' it is marked as visited*/
				if(arr[source]==0)
				{
					enqueue(q,source);
					arr[source]=1;
				}
				e=e->next_edge;
			}	
		}
	printf("not yet implemented: put code for part 2 here\n");
}


/****************************************************************/


void detailed_path(Graph* graph, int source_id, int destination_id) {
	/* the array 'arr' stores that a particular node was visited or not*/
	
	/*'is_found' is used as flag to keep a check on whether we have reached
	the destination or not */
	
	/*'w' is used to store the distance between nodes*/
	
	int arr[graph->n],i,id=source_id,source,w=0,is_found=0;
	
	/*the for loop initialises all indexes of the array 'arr' to zero because
	initially you want to imply that no node is visited*/
	
	for(i=0;i<graph->n;i++)
	{
		arr[i]=0;
	}
	List *s=new_stack(); 
	
	stack_push(s,source_id); //pushes the first node onto the stack
	arr[id]=1;
	/*the while loop executes till the the destination is reached by travesing
	through the graph using a depth-first traversal*/
	while(!is_found)
	{
		id=stack_pop(s);/*removes the first node from the queue and stores its 
		                  id*/
		printf("%s (%d)\n",graph->vertices[id]->label,w);
	    Edge *e=graph->vertices[id]->first_edge;/* 'e' stores the address of 
		                             the adjacent edges to the node at 'id'*/
		
		while(e!=NULL)
		{
			source=e->v;
			
				
			/*if the destination is reached the flag is set to true 
			and then we break out of the inner while loop*/
			if(source==destination_id)
			{
				is_found=1;
				printf("%s (%d)\n",graph->vertices[source]->label,(w+=e->weight));
				break;
			}
				
			/*if the node at 'source' is not visited it is placed on
			the stack, in array 'arr' it is marked as visited and 
			also 'w' is incremented with the distance between
			those particular nodes*/
			if(arr[source]==0)
			{
				stack_push(s,source);
				arr[source]=1;
				w+=e->weight; 
			}
			e=e->next_edge;
		}	
	}
	
	
	printf("not yet implemented: put code for part 3 here\n");
}


/****************************************************************/


void all_paths(Graph* graph, int source_id, int destination_id) {
	/* the array 'arr' stores that a particular node was visited or not */
	/*the array 'path' stores the path from the source to destination*/
	
	int arr[graph->n], i, path[graph->n], path_index =0;
	  
    /*the for loop initialises all indexes of the array 'arr' to zero because
	initially you want to imply that no node is visited*/
	for(i=0;i<graph->n;i++)
	{
		arr[i]=0;
	}
	
	print_all_paths(graph,source_id, destination_id, arr, path, &path_index);
	

	printf("not yet implemented: put code for part 4 here\n");
}



/*This function is used to print the current path and also traverse 
through the graph to find the other possible paths*/
void print_all_paths(Graph* graph,int source, int d, int visited[],
                              int path[], int *path_index)
{
	int i, source1;
    path[*path_index] = source; //You store the node at source into the 'path' 
    (*path_index)++;
    visited[source] = 1;
    
    /*if the current node is same as the destination then print the path 
    using the array 'path'*/
    if (source==d)
    {
        for ( i=0; i<((*path_index)-1); i++)
        {
        	printf("%s,",graph->vertices[path[i]]->label);
        	
        }
        	printf("%s",graph->vertices[path[i]]->label);
            printf("\n");
        
    }
    /*if the current node is not the destination then you traverse 
    through all the nodes adjacent to the current node*/
    else 
    {
        
        Edge *e=graph->vertices[source]->first_edge;/* 'e' stores the address of 
		                             the adjacent edges to the node at 'id'*/
        //printf("source=%d city=%s\n",source,graph->vertices[source]->label);
        while(e!=NULL)
        {
        	source1=e->v;
        	/*if the current node is not visited before then try searching
            for a path from there*/
        	if (!visited[source1])
        	{
        		print_all_paths(graph,source1, d, visited, path, path_index);
        	}
        	e=e->next_edge;     
        }         
    }
 
    //remove the current node from the array 'path' and mark it as unvisited
    (*path_index)--;
    visited[source]=0;
}


/****************************************************************/


void shortest_path(Graph* graph, int source_id, int destination_id) {
	/* the array 'arr' stores that a particular node was visited or not,
	'w' stores the distance from a source to the destination,
	the array 'path' stores the path from the source to destination,
	the array 'min_path' stores the shortest path 
	shortest_path_index' works as a counter for 'min_path'*/
	
	int arr[graph->n], i, w=0, path[graph->n], min_path[graph->n],
	shortest_path_index=0;
	int path_index=0,min=10000;
	
	
	/*the for loop initialises all indexes of the array 'arr' to zero because
	initially you want to imply that no node is visited*/
	for(i=0;i<graph->n;i++)
	{
		arr[i]=0;
	}
	find_shortest_path(graph,source_id, destination_id, arr, w, &path_index,
		&min,path,min_path,&shortest_path_index);
	
	/*the for loop prints out the shortest path using the array 'min_path' */
	for ( i=0; i<(shortest_path_index-1); i++)
        {
        	printf("%s,",graph->vertices[min_path[i]]->label);
        	
        }
        
    printf("%s",graph->vertices[min_path[i]]->label);      
	printf(" (%d)\n",min);
	printf("not yet implemented: put code for part 5 here\n");
}


/*the function recursively travesres through all the possible paths and finds
the shortest among them */
void find_shortest_path(Graph* graph,int source, int d, int visited[],
                              int w, int *path_index,int *min,int path[],
                              int min_path[],int *shortest_path_index)
{
	int i, source1, temp;
    path[*path_index] = source;//You store the node at source into the 'path' 
    (*path_index)++;
    visited[source] = 1;
     /*if the current node is same as the destination and if the path is smaller
     than the previous one then store its details */
    if (source==d)
    {
           if(w<=(*min))
           {
           	  for ( i=0; i<(*path_index); i++)
           	  {
           	  	  min_path[i]=path[i];
        	
        	  }
        	  (*shortest_path_index)=*path_index;
           	   (*min)=w;
           }
        //printf("w=%d  min=%d\n",w,*min);
    }
    
    /*if the current node is not the destination then you traverse 
    through all the nodes adjacent to the current node*/
    else 
    {
        
        Edge *e=graph->vertices[source]->first_edge;/* 'e' stores the address of 
		                             the adjacent edges to the node at 'id'*/
        
        //printf("source=%d city=%s\n",source,graph->vertices[source]->label);
        while(e!=NULL)
        {
        	source1=e->v;
        	/*if the current node is not visited then search for a path from 
        	there and also store the distance and then substract it after you 
        	are done with that node*/
        	if (visited[source1]==0)
        	{
        		temp=w+(e->weight);
        		//arr_w[*path_index] = e->next_edge->weight;
        		find_shortest_path(graph,source1, d, visited, temp, path_index,min,
        			path,min_path,shortest_path_index);
        	}
        	e=e->next_edge;
                
        }
              
    }
 
    //remove the current node from the array 'path' and mark it as unvisited
    (*path_index)--;
    visited[source] = 0;
}

