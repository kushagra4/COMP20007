/* * * * * * * * *
 * Dynamic hash table using cuckoo hashing, resolving collisions by switching
 * keys between two tables with two separate hash functions
 *
 * created for COMP20007 Design of Algorithms - Assignment 2, 2017
 * by ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "cuckoo.h"

#define MAXSTEPS 20
// an inner table represents one of the two internal tables for a cuckoo
// hash table. it stores two parallel arrays: 'slots' for storing keys and
// 'inuse' for marking which entries are occupied
typedef struct inner_table {
	int64 *slots;	// array of slots holding keys
	bool  *inuse; // is this slot in use or not?
} InnerTable;

// a cuckoo hash table stores its keys in two inner tables
struct cuckoo_table {
	InnerTable *table1; // first table
	InnerTable *table2; // second table
	int size;			// size of each table
};



InnerTable *initialise_table(InnerTable *in_table, int size) {
	assert(size < MAX_TABLE_SIZE && "error: table has grown too large!");

	in_table->slots = malloc((sizeof *in_table->slots) * size);
	assert(in_table->slots);
	in_table->inuse = malloc((sizeof *in_table->inuse) * size);
	assert(in_table->inuse);
	int i;
	for (i = 0; i < size; i++) {
		in_table->inuse[i] = false;
	}

	return in_table;
}








// initialise a cuckoo hash table with 'size' slots in each table
CuckooHashTable *new_cuckoo_hash_table(int size) {
	//printf("here\n");
	CuckooHashTable *table=malloc(sizeof *table);
	assert(table!=NULL);
	table->table1=malloc(sizeof *table->table1);
	assert((table->table1)!=NULL);
	table->table2=malloc(sizeof *table->table2);
	assert((table->table2)!=NULL);
	initialise_table(table->table1,size);
	initialise_table(table->table2,size);
	
	table->size=size;
	//fprintf(stderr, "not yet implemented\n");
	return table;
}


/*InnerTable *double_inner_table(CuckooHashTable *table,InnerTable *in_table, int size) {
	int64 *oldslots = in_table->slots;
	bool  *oldinuse = in_table->inuse;
	int oldsize = size;

	initialise_table(in_table, size*2);

	int i;
	for (i = 0; i < oldsize; i++) {
		if (oldinuse[i] == true) {
			cuckoo_hash_table_insert(table, oldslots[i]);
		}
	}

	free(oldslots);
	free(oldinuse);
	return in_table;
}*/


static void double_table(CuckooHashTable *table) {
	int64 *oldslots1=table->table1->slots;
	bool *oldinuse1= table->table1->inuse;
	int64 *oldslots2=table->table2->slots;
	bool *oldinuse2= table->table2->inuse;
	int oldsize=table->size,i;
	
	table->table1=initialise_table(table->table1, 2*oldsize);
	table->table2=initialise_table(table->table2, 2*oldsize);
	table->size=2*(table->size);
	for (i = 0; i < oldsize; i++) {
		if (oldinuse1[i] == true) {
			cuckoo_hash_table_insert(table, oldslots1[i]);
		}
	}
	
	//printf("here1\n");
	for (i = 0; i < oldsize; i++) {
		if (oldinuse2[i] == true) {
			cuckoo_hash_table_insert(table, oldslots2[i]);
		}
	}
	
	//printf("here2\n");
	
}




// free all memory associated with 'table'
void free_cuckoo_hash_table(CuckooHashTable *table) {
	free(table->table1->slots);
	free(table->table2->slots);
	free(table->table1->inuse);
	free(table->table2->inuse);
	free(table->table1);
	free(table->table2);
	free(table);
	//fprintf(stderr, "not yet implemented\n");
}

int toogle_hash(int t,int64 key, int size)
{
	switch(t)
	{
		case 1: return (h1(key) % size);
		break;
		
		case 2: return (h2(key) % size);
	}
	return 0;
}




// insert 'key' into 'table', if it's not in there already
// returns true if insertion succeeds, false if it was already in there
bool cuckoo_hash_table_insert(CuckooHashTable *table, int64 key) {
	assert(table!=NULL);
	int steps=1,t=1,v;
	//bool insert=false;
	int64 new_key;
	
	
	//printf("size=%d  steps=%d\n",table->size,steps);
	while(steps<=MAXSTEPS)
	{
		t=1;
		
		if(t==1)
		{
			v=toogle_hash(t,key,table->size);
			if( cuckoo_hash_table_lookup(table,key) )
			{
				return false;
			}
			else if(!(table->table1->inuse[v]))
			{
				table->table1->slots[v]=key;
				table->table1->inuse[v]=true;
				//cuckoo_hash_table_print(table);
				return true;
			}
			
			else
			{
				new_key=table->table1->slots[v];
				table->table1->slots[v]=key;
				//cuckoo_hash_table_print(table);
				key=new_key;
				t=2;
			}
			steps++;
		}
		
		if(t==2)
		{
			v=toogle_hash(t,key,table->size);
			if( cuckoo_hash_table_lookup(table,key) )
			{
				return false;
			}
			else if(!(table->table2->inuse[v]))
			{
				table->table2->slots[v]=key;
				table->table2->inuse[v]=true;
				//cuckoo_hash_table_print(table);
				return true;
			}
			
			else
			{
				new_key=table->table2->slots[v];
				table->table2->slots[v]=key;
				//cuckoo_hash_table_print(table);
				key=new_key;
				t=1;
			}
			steps++;
		}
		
	}
	//printf("size=%d  steps=%d\n",table->size,steps);
	if(steps>=MAXSTEPS)
	{
		steps=1;
		double_table(table);
		//printf("here3\n");
		cuckoo_hash_table_insert(table,key);
	}
	//fprintf(stderr, "not yet implemented\n");
	return true;
}





// lookup whether 'key' is inside 'table'
// returns true if found, false if not
bool cuckoo_hash_table_lookup(CuckooHashTable *table, int64 key) {
	int size=table->size;
	int v1=h1(key) % size, v2=h2(key) % size;
	if( (table->table1->slots[v1])==key || (table->table2->slots[v2])==key)
	{
		return true;
	}
	//fprintf(stderr, "not yet implemented\n");
	return false;
}





// print the contents of 'table' to stdout
void cuckoo_hash_table_print(CuckooHashTable *table) {
	assert(table);
	printf("--- table size: %d\n", table->size);

	// print header
	printf("                    table one         table two\n");
	printf("                  key | address     address | key\n");
	
	// print rows of each table
	int i;
	for (i = 0; i < table->size; i++) {

		// table 1 key
		if (table->table1->inuse[i]) {
			printf(" %20llu ", table->table1->slots[i]);
		} else {
			printf(" %20s ", "-");
		}

		// addresses
		printf("| %-9d %9d |", i, i);

		// table 2 key
		if (table->table2->inuse[i]) {
			printf(" %llu\n", table->table2->slots[i]);
		} else {
			printf(" %s\n",  "-");
		}
	}

	// done!
	printf("--- end table ---\n");
}


// print some statistics about 'table' to stdout
void cuckoo_hash_table_stats(CuckooHashTable *table) {
	int i,size=table->size,n1=0,n2=0;
	
	for(i=0;i<size;i++)
	{
		if(table->table1->inuse[i])
		{
			n1++;
		}
	}
	for(i=0;i<size;i++)
	{
		if(table->table2->inuse[i])
		{
			n2++;
		}
	}
	printf("load1=%d  load2=%d\n",n1,n2);
	//fprintf(stderr, "not yet implemented\n");
}
