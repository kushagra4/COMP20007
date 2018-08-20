/* * * * * * * * *
* Dynamic hash table using a combination of extendible hashing and cuckoo
* hashing with a single keys per bucket, resolving collisions by switching keys 
* between two tables with two separate hash functions and growing the tables 
* incrementally in response to cycles
*
* created for COMP20007 Design of Algorithms - Assignment 2, 2017
* by ...
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xuckoo.h"
#define rightmostnbits(n, x) (x) & ((1 << (n)) - 1)
// a bucket stores a single key (full=true) or is empty (full=false)
// it also knows how many bits are shared between possible keys, and the first 
// table address that references it
typedef struct bucket {
	int id;		// a unique id for this bucket, equal to the first address
				// in the table which points to it
	int depth;	// how many hash value bits are being used by this bucket
	bool full;	// does this bucket contain a key
	int64 key;	// the key stored in this bucket
} Bucket;

// an inner table is an extendible hash table with an array of slots pointing 
// to buckets holding up to 1 key, along with some information about the number 
// of hash value bits to use for addressing
typedef struct inner_table {
	Bucket **buckets;	// array of pointers to buckets
	int size;			// how many entries in the table of pointers (2^depth)
	int depth;			// how many bits of the hash value to use (log2(size))
	int nkeys;			// how many keys are being stored in the table
} InnerTable;

// a xuckoo hash table is just two inner tables for storing inserted keys
struct xuckoo_table {
	InnerTable *table1;
	InnerTable *table2;
};


static Bucket *new_bucket(int first_address, int depth) {
	Bucket *bucket = malloc(sizeof *bucket);
	assert(bucket);

	bucket->id = first_address;
	bucket->depth = depth;
	bucket->full = false;

	return bucket;
}


static void initialise_table(InnerTable *in_table, int size) {
	assert(size < MAX_TABLE_SIZE && "error: table has grown too large!");

	in_table->buckets = malloc((sizeof *in_table->buckets) * size);
	assert(in_table->buckets);
	in_table->size=size;
	in_table->depth=0;
	in_table->nkeys=0;
	in_table->buckets[0]=new_bucket(0,0);
	
}


int xtoogle_hash(int t,int64 key, int depth)
{
	switch(t)
	{
		case 1: return rightmostnbits(depth, h1(key));
		break;
		
		case 2: return rightmostnbits(depth, h2(key));
		break;
	}
	return 0;
}


static void double_table(XuckooHashTable *table,InnerTable *in_table, int t) {
	int size = in_table->size * 2;
	printf("%d %d",size,t);
	assert(size < MAX_TABLE_SIZE && "error: table has grown too large!");

	// get a new array of twice as many bucket pointers, and copy pointers down
	in_table->buckets = realloc(in_table->buckets, (sizeof *in_table->buckets) * size);
	assert(in_table->buckets);
	//table->table2->buckets = realloc(table->table2->buckets, (sizeof *table->table2->buckets) * size);
	//assert(table->table2->buckets);
	int i;
	for (i = 0; i < in_table->size; i++) {
		in_table->buckets[in_table->size + i] = in_table->buckets[i];
		//table->table2->buckets[table->table2->size + i] = table->table2->buckets[i];
	}

	// finally, increase the table size and the depth we are using to hash keys
	in_table->size = size;
	in_table->depth++;
	//table->table2->size = size;
	//table->table2->depth++;
	if(t==1){
		table->table1=in_table;
	}
	else{
		table->table2=in_table;
	}
}



static void reinsert_key(XuckooHashTable *table,InnerTable *in_table, int64 key, int t) {
	
	int address = xtoogle_hash(t,key,in_table->depth);
	//xuckoo_hash_table_insert(table, key);
	in_table->buckets[address]->key = key;
	in_table->buckets[address]->full = true;
	if(t==1){
		table->table1=in_table;
	}
	else{
		table->table2=in_table;
	}
}


static void split_bucket(XuckooHashTable *table, InnerTable *in_table, int address,int t) {
	
	if (in_table->buckets[address]->depth == in_table->depth) {
		// yep, this bucket is down to its last pointer
		 double_table(table,in_table, t);
	}
	// either way, now it's time to split this bucket


	// SECOND,
	// create a new bucket and update both buckets' depth
	Bucket *bucket = in_table->buckets[address];
	int depth = bucket->depth;
	int first_address = bucket->id;

	int new_depth = depth + 1;
	bucket->depth = new_depth;

	// new bucket's first address will be a 1 bit plus the old first address
	int new_first_address = 1 << depth | first_address;
	Bucket *newbucket = new_bucket(new_first_address, new_depth);
	//table->stats.nbuckets++;
	
	// THIRD,
	// redirect every second address pointing to this bucket to the new bucket
	// construct addresses by joining a bit 'prefix' and a bit 'suffix'
	// (defined below)

	// suffix: a 1 bit followed by the previous bucket bit address
	int bit_address = rightmostnbits(depth, first_address);
	int suffix = (1 << depth) | bit_address;

	// prefix: all bitstrings of length equal to the difference between the new
	// bucket depth and the table depth
	// use a for loop to enumerate all possible prefixes less than maxprefix:
	int maxprefix = 1 << (in_table->depth - new_depth);

	int prefix;
	for (prefix = 0; prefix < maxprefix; prefix++) {
		
		// construct address by joining this prefix and the suffix
		int a = (prefix << new_depth) | suffix;

		// redirect this table entry to point at the new bucket
		in_table->buckets[a] = newbucket;
	}

	// FINALLY,
	// filter the key from the old bucket into its rightful place in the new 
	// table (which may be the old bucket, or may be the new bucket)

	// remove and reinsert the key
	int64 key = bucket->key;
	bucket->full = false;
	reinsert_key(table,in_table, key,t);
	if(t==1){
		table->table1=in_table;
	}
	else{
		table->table2=in_table;
	}
}




// initialise an extendible cuckoo hash table
XuckooHashTable *new_xuckoo_hash_table() {
	XuckooHashTable *table=malloc(sizeof(*table));
	assert(table);
	table->table1=malloc(sizeof(*table->table1));
	assert(table->table1);
	table->table2=malloc(sizeof(*table->table2));
	assert(table->table2);
	initialise_table(table->table1,1);
	initialise_table(table->table2,1);
	//fprintf(stderr, "not yet implemented\n");
	return table;
}


// free all memory associated with 'table'
void free_xuckoo_hash_table(XuckooHashTable *table) {
	fprintf(stderr, "not yet implemented\n");
}






// insert 'key' into 'table', if it's not in there already
// returns true if insertion succeeds, false if it was already in there
bool xuckoo_hash_table_insert(XuckooHashTable *table, int64 key) {
	assert(table);
	int steps=1, t=1, v,maxsteps=(table->table1->size)+(table->table2->size);
	int64 new_key;
	if(table->table1->nkeys > table->table2->nkeys)
	{
			t=2;
	}
	while(steps<=maxsteps)
	{
		
		
		if(t==1)
		{
			v=xtoogle_hash(t,key,table->table1->depth);
			if( xuckoo_hash_table_lookup(table,key) )
			{
				return false;
			}
			else if(!(table->table1->buckets[v]->full))
			{
				table->table1->buckets[v]->key=key;
				table->table1->buckets[v]->full=true;
				table->table1->buckets[v]->id=v;
				table->table1->nkeys++;
				//cuckoo_hash_table_print(table);
				return true;
			}
			
			else
			{
				new_key=table->table1->buckets[v]->key;
				table->table1->buckets[v]->key=key;
				//cuckoo_hash_table_print(table);
				key=new_key;
				t=2;
				//printf("new_key2=%d\n",new_key);
			}
			steps++;
			if(steps==maxsteps)
			{
				break;
			}
		}
		
		if(t==2)
		{
			v=xtoogle_hash(t,key,table->table2->depth);
			if( xuckoo_hash_table_lookup(table,key) )
			{
				return false;
			}
			else if(!(table->table2->buckets[v]->full))
			{
				table->table2->buckets[v]->key=key;
				table->table2->buckets[v]->full=true;
				table->table2->buckets[v]->id=v;
				table->table2->nkeys++;
				//cuckoo_hash_table_print(table);
				return true;
			}
			
			else
			{
				new_key=table->table2->buckets[v]->key;
				table->table2->buckets[v]->key=key;
				//cuckoo_hash_table_print(table);
				key=new_key;
				t=1;
				//printf("new_key2=%d\n",new_key);
			}
			steps++;
			if(steps==maxsteps)
			{
				break;
			}
		}
		
	}
	//printf("key=%d\n",key);
	/*int v1=xtoogle_hash(1,key,table->table1->depth),
	v2=xtoogle_hash(2,key,table->table2->depth),
	diff1=(table->table1->depth)-(table->table1->buckets[v1]->depth),
	diff2=(table->table2->depth)-(table->table2->buckets[v2]->depth);
	if(diff1==diff2)
	{
		double_table(table);
		if(table->table1->nkeys > table->table2->nkeys)
		{
			while (table->table2->buckets[v2]->full) 
			{
				split_bucket(table,table->table2,v2,2);
				v2 = rightmostnbits(table->table2->depth, h2(key));
			}
			table->table2->buckets[v2]->key = key;
			table->table2->buckets[v2]->full = true;
			table->table2->nkeys++;
			return true;
			
		}
		else if(table->table1->nkeys < table->table2->nkeys)
		{
			while (table->table1->buckets[v1]->full) 
			{
				split_bucket(table,table->table1,v1,1);
				v1= rightmostnbits(table->table1->depth, h1(key));
			}
			table->table1->buckets[v1]->key = key;
			table->table1->buckets[v1]->full = true;
			table->table1->nkeys++;
			return true;
		}
		else
		{
			while (table->table1->buckets[v1]->full) 
			{
				split_bucket(table,table->table1,v1,1);
				v1 = rightmostnbits(table->table1->depth, h1(key));
			}
			table->table1->buckets[v1]->key = key;
			table->table1->buckets[v1]->full = true;
			table->table1->nkeys++;
			return true;
		}
	}*/
	/*int fl=0;
	while(1==1)
	{
		if(diff1 >= diff2)
		{
			while (table->table1->buckets[v1]->full) 
			{
				//xuckoo_hash_table_print(table);
				split_bucket(table,table->table1,v1,1);
				v1 = rightmostnbits(table->table1->depth, h1(key));
				v1=xtoogle_hash(1,key,table->table1->depth),
				v2=xtoogle_hash(2,key,table->table1->depth),
			    diff1=(table->table1->depth)-(table->table1->buckets[v1]->depth),
				diff2=(table->table2->depth)-(table->table2->buckets[v1]->depth);
				if(diff1<diff2)
				{
					fl=1;
					break;
				}
			}
			if(fl==0){
			table->table1->buckets[v1]->key = key;
			table->table1->buckets[v1]->full = true;
			table->table1->nkeys++;
			return true;
			}
			fl=0;
		}
	if(diff1 <=diff2)
	{
		while (table->table2->buckets[v2]->full) 
			{
				//xuckoo_hash_table_print(table);
				split_bucket(table,table->table2,v2,2);
				v2 = rightmostnbits(table->table2->depth, h2(key));
				v1=xtoogle_hash(1,key,table->table1->depth),
				v2=xtoogle_hash(2,key,table->table1->depth),
			    diff1=(table->table1->depth)-(table->table1->buckets[v1]->depth),
				diff2=(table->table2->depth)-(table->table2->buckets[v1]->depth);
				if(diff1>diff2)
				{fl=1;
					break;
				}
			}
			if(fl==0){
			table->table2->buckets[v2]->key = key;
			table->table2->buckets[v2]->full = true;
			table->table2->nkeys++;
			return true;}
			fl=0;
	}
	}*/
	
	int v1=xtoogle_hash(1,key,table->table1->depth),
	v2=xtoogle_hash(2,key,table->table2->depth);
	int size1=table->table1->size, size2=table->table2->size;
	
	
	int diff1=(table->table1->depth)-(table->table1->buckets[v1]->depth),
	diff2=(table->table2->depth)-(table->table2->buckets[v2]->depth);
	//printf("diff1=%d diff2=%d", diff1, diff2);

	//int counter=0;
	int flag=0;
	int flag1=1;
	
	printf("t=%d\n",t);
	while(1==1)
	{ 
		
		
		printf("fhyjy");
		printf("\ndiff1=%d\n",diff1);
		printf("diff2=%d\n",diff2);
		printf("size1=%d\n",size1);
		printf("size2=%d\n",size2);
			printf("t=%d\n",t);
		if(!table->table1->buckets[v1]->full)
		{
			table->table1->buckets[v1]->key=key;
			table->table1->buckets[v1]->full=true;
			table->table1->nkeys++;
			return true;
		}
		if(!table->table2->buckets[v2]->full)
		{
			table->table2->buckets[v2]->key=key;
			table->table2->buckets[v2]->full=true;
			table->table2->nkeys++;
			return true;
		}
		if(flag1==0 ||(diff2!=0 && diff1==0)||(diff2> diff1)||((diff1-diff2==0)&&(table->table2->size < table->table1->size)))
		{
			flag1=1;
			flag=0;
			while (table->table2->buckets[v2]->full ) {
				split_bucket(table,table->table2,v2,2);
				printf("f");
			
				v1 = rightmostnbits(table->table1->depth, h1(key));
				v2 = rightmostnbits(table->table2->depth, h2(key));
				
				diff1=(table->table1->depth)-(table->table1->buckets[v1]->depth),
	            diff2=(table->table2->depth)-(table->table2->buckets[v2]->depth);
	            if(!table->table2->buckets[v2]->full)
	            {
	            	table->table2->buckets[v2]->key=key;
	            	table->table2->buckets[v2]->full=true;
	            	table->table2->nkeys++;
	            	return true;
				}
				
				if((diff2==0 && diff1!=0)|| (((diff1-diff2==0)&&(table->table2->size >= table->table1->size))|| (diff1>diff2)))
				{ 
			
					flag=1;
					flag1=1;
					
					break;
				}
				
			}
		}
	 if((flag==1)||(diff1!=0 && diff2==0)||(diff1> diff2)||((diff1-diff2==0)&&(table->table2->size >= table->table1->size)))
		{
			
	
			printf("t=%d\n",t);
			flag=0;
			flag1=1;
			while (table->table1->buckets[v1]->full ) {
		//	xuckoo_hash_table_print(table) ;
				split_bucket(table,table->table1,v1,1);
				
				printf("11111f");
				v1 = rightmostnbits(table->table1->depth, h1(key));
				v2 = rightmostnbits(table->table2->depth, h2(key));
				
				diff1=(table->table1->depth)-(table->table1->buckets[v1]->depth),
	            diff2=(table->table2->depth)-(table->table2->buckets[v2]->depth);
	            if(!table->table1->buckets[v1]->full)
	            {
	            	table->table1->buckets[v1]->key=key;
	            	table->table1->buckets[v1]->full=true;
	            	table->table1->nkeys++;
	            	return true;
				}
				printf("\ndiff1=%d\n",diff1);
	
				if((diff1==0 && diff2!=0) || ((diff1-diff2==0)&&(table->table2->size < table->table1->size))|| (diff2>diff1))
				{ 
				
					flag1=0;
					flag=0;
					break;
				}
				
			}
		}
	}
		
			
			/*while (table->table2->buckets[v2]->full) {
			//printf("here\n");
			
	
	
				split_bucket(table,table->table2,v2,2);
				
				v2 = rightmostnbits(table->table2->depth, h2(key));
				v1 = rightmostnbits(table->table1->depth, h1(key));
				printf("after splitting size dfff%d\n",table->table2->buckets[v2]->depth);
				if(table->table2->buckets[v2]->depth==table->table2->depth){
					change=change+1;
					printf("\n%d change\n",change);
				}

				if(!table->table2->buckets[v2]->full)
				{
				table->table2->buckets[v2]->key=key;
				table->table2->buckets[v2]->full=true;
				return true;
				}
				size1=table->table1->size, size2=table->table2->size;
				if(change>=2)
				{	printf("in 2\n");
					printf("come\n");
					change=0;
					t=1;
					break;
				}
			}
			
		}
	}*/
		
		
		

	
	
	
	//fprintf(stderr, "not yet implemented\n");
	return true;
}


// lookup whether 'key' is inside 'table'
// returns true if found, false if not
bool xuckoo_hash_table_lookup(XuckooHashTable *table, int64 key) {
	int v1= rightmostnbits(table->table1->depth, h1(key));
	int v2 = rightmostnbits(table->table2->depth, h2(key));
	if( (table->table1->buckets[v1]->key)==key || (table->table2->buckets[v2]->key)==key)
	{
		return true;
	}
	//fprintf(stderr, "not yet implemented\n");
	return false;
}


// print the contents of 'table' to stdout
void xuckoo_hash_table_print(XuckooHashTable *table) {
	assert(table != NULL);

	printf("--- table ---\n");

	// loop through the two tables, printing them
	InnerTable *innertables[2] = {table->table1, table->table2};
	int t;
	for (t = 0; t < 2; t++) {
		// print header
		printf("table %d\n", t+1);

		printf("  table:               buckets:\n");
		printf("  address | bucketid   bucketid [key]\n");
		
		// print table and buckets
		int i;
		for (i = 0; i < innertables[t]->size; i++) {
			// table entry
			printf("%9d | %-9d ", i, innertables[t]->buckets[i]->id);

			// if this is the first address at which a bucket occurs, print it
			if (innertables[t]->buckets[i]->id == i) {
				printf("%9d ", innertables[t]->buckets[i]->id);
				if (innertables[t]->buckets[i]->full) {
					printf("[%llu]", innertables[t]->buckets[i]->key);
				} else {
					printf("[ ]");
				}
			}

			// end the line
			printf("\n");
		}
	}
	printf("--- end table ---\n");
}


// print some statistics about 'table' to stdout
void xuckoo_hash_table_stats(XuckooHashTable *table) {
	fprintf(stderr, "not yet implemented\n");
	return;
}
