/* * * * * * * * *
 * Dynamic hash table using extendible hashing with multiple keys per bucket,
 * resolving collisions by incrementally growing the hash table
 *
 * created for COMP20007 Design of Algorithms - Assignment 2, 2017
 * by ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xtndbln.h"
#define rightmostnbits(n, x) (x) & ((1 << (n)) - 1)
// a bucket stores an array of keys
// it also knows how many bits are shared between possible keys, and the first 
// table address that references it
typedef struct xtndbln_bucket {
	int id;			// a unique id for this bucket, equal to the first address
					// in the table which points to it
	int depth;		// how many hash value bits are being used by this bucket
	int nkeys;		// number of keys currently contained in this bucket
	int64 *keys;	// the keys stored in this bucket
} Bucket;

// a hash table is an array of slots pointing to buckets holding up to 
// bucketsize keys, along with some information about the number of hash value 
// bits to use for addressing
struct xtndbln_table {
	Bucket **buckets;	// array of pointers to buckets
	int size;			// how many entries in the table of pointers (2^depth)
	int depth;			// how many bits of the hash value to use (log2(size))
	int bucketsize;		// maximum number of keys per bucket
};



static Bucket *new_bucket(int first_address, int depth,int bucketsize) {
	Bucket *bucket = malloc((sizeof *bucket));
	assert(bucket);
	
	bucket->id = first_address;
	bucket->depth = depth;
	bucket->nkeys=0;
	bucket->keys=malloc(bucketsize*(sizeof bucket->keys));
	assert(bucket->keys);

	return bucket;
}

static void initialise_bucket(XtndblNHashTable *table)
{
	int i=0;
	for(i=0;i<(table->size);i++)
	{
		table->buckets[i]=new_bucket(0,0,table->bucketsize);
	}
}
// initialise an extendible hash table with 'bucketsize' keys per bucket
XtndblNHashTable *new_xtndbln_hash_table(int bucketsize) {
	XtndblNHashTable *table=malloc(sizeof(*table));
	assert(table);
	table->size=1;
	table->depth=0;
	table->bucketsize=bucketsize;
	table->buckets=malloc(sizeof(table->buckets)*table->size);
	assert(table->buckets);
	initialise_bucket(table);
	fprintf(stderr, "not yet implemented\n");
	return table;
}


// free all memory associated with 'table'
void free_xtndbln_hash_table(XtndblNHashTable *table) {
	/*int i;
	for(i=0;i <table->size; i++)
	{
		free(table->buckets[i]->keys);
		free(table->buckets[i]);
	}
	free(table->buckets);
	free(table);*/
	fprintf(stderr, "not yet implemented\n");
}




XtndblNHashTable *double_table_n(XtndblNHashTable *table) {
	int size = table->size * 2;
	assert(size < MAX_TABLE_SIZE && "error: table has grown too large!");

	// get a new array of twice as many bucket pointers, and copy pointers down
	table->buckets = realloc(table->buckets, (sizeof *table->buckets) * size);
	assert(table->buckets);
	//xtndbln_hash_table_print(table);
	//printf("inside double bucket[0]->keys=%p\n",table->buckets[0]->keys);
	int i;
	/*for(i=0;i< table->size; i++)
	{
		table->buckets[table->size+i]=new_bucket((table->size+i), table->depth,table->bucketsize);
	}*/
	/*int j;
	for (j = 0; j < table->size; j++) {
        Bucket *bucket = malloc(sizeof *bucket);
        table->buckets[j+table->size]=bucket;
        table->buckets[j+table->size]->keys=malloc(sizeof(bucket->keys)*table->bucketsize);
        }*/
	for (i = 0; i < table->size; i++) {
		table->buckets[table->size+i]= new_bucket(0,0,table->bucketsize);
		table->buckets[table->size + i] = table->buckets[i];
	}
	//xtndbln_hash_table_print(table);
	// finally, increase the table size and the depth we are using to hash keys
	table->size = size;
	table->depth++;
	return table;
}





static void reinsert_key(XtndblNHashTable *table, int64 key) {
	int address = rightmostnbits(table->depth, h1(key));
	printf("address_reinsert=%d  key=%d\n",address, key);
	table->buckets[address]->keys[(table->buckets[address]->nkeys)++] = key;
	//xtndbln_hash_table_print(table);
	//printf("table->buckets[address]->nkeys=%d\n",table->buckets[address]->nkeys);
	
}

static void split_bucket(XtndblNHashTable *table, int address)
{
	//printf("bucket[0]->keys=%p\n",table->buckets[0]->keys);
	printf("address=%d\n",address);
	printf("table->buckets[address]->depth=%d  table->depth=%d\n",table->buckets[address]->depth,table->depth);
	if(table->buckets[address]->depth == table->depth)
	{
		table=double_table_n(table);
	}
	printf("here4\n");
	printf("table=%p\n",table);
	//xtndbln_hash_table_print(table);
	Bucket *bucket=table->buckets[address];
	int64 *reinsert=malloc(bucket->nkeys*(sizeof(*reinsert)));
	int i,n_reinsert=bucket->nkeys;
	for(i =0;i < n_reinsert;i++)
	{
		reinsert[i]=bucket->keys[i];
	}
	int depth = bucket->depth;
	int first_address = bucket->id;

	int new_depth = depth + 1;
	//bucket->depth = new_depth;
	//printf("address_new=%d\n",address);
	//printf("first_address=%d  depth=%d\n",first_address,depth);
	free(bucket->keys);
	free(bucket);
	table->buckets[address]=new_bucket(address,new_depth, table->bucketsize);
	assert(table->buckets[address]);
	
	int new_first_address = 1 << depth | first_address;
	printf("new_first_address=%d\n",new_first_address);
	Bucket *newbucket = new_bucket(new_first_address, new_depth,table->bucketsize);
	assert(newbucket);
	//(table->size)++;
	
	
	// suffix: a 1 bit followed by the previous bucket bit address
	int bit_address = rightmostnbits(depth, first_address);
	int suffix = (1 << depth) | bit_address;

	// prefix: all bitstrings of length equal to the difference between the new
	// bucket depth and the table depth
	// use a for loop to enumerate all possible prefixes less than maxprefix:
	int maxprefix = 1 << (table->depth - new_depth);

	int prefix;
	for (prefix = 0; prefix < maxprefix; prefix++) {
		
		// construct address by joining this prefix and the suffix
		int a = (prefix << new_depth) | suffix;
		//printf("a=%d\n",a);
		// redirect this table entry to point at the new bucket
		table->buckets[a] = newbucket;
	}
	
		// FINALLY,
	// filter the key from the old bucket into its rightful place in the new 
	// table (which may be the old bucket, or may be the new bucket)

	// remove and reinsert the key
	//int64 key = bucket->key;
	//bucket->full = false;
	//reinsert_key(table, key);
	/*for(i=0; i<bucket_size;i++)
	{
		table->buckets[address]->keys[i]=NULL;
	}*/
	//printf("bucket_size=%d\n",n_reinsert);
	
	for(i=0; i<n_reinsert;i++)
	{
		reinsert_key(table,reinsert[i]);
	}
	printf("here5\n");
	//xtndbln_hash_table_print(table);
	free(reinsert);
	//free(bucket->keys);
	//free(bucket);
}



// insert 'key' into 'table', if it's not in there already
// returns true if insertion succeeds, false if it was already in there
bool xtndbln_hash_table_insert(XtndblNHashTable *table, int64 key) {
	assert(table);
	int hash=h1(key);
	int address=rightmostnbits(table->depth,hash);
	printf("address=%d\n",address);
	if(xtndbln_hash_table_lookup(table, key))
	{
		return false;
	}
	printf("here0\n");
	//printf("nkeys1=%d\n",(table->buckets[address]->nkeys));
	while(table->buckets[address]->nkeys==table->bucketsize)
	{
		split_bucket(table, address);
		//printf("back from split_bucket\n");
		address= rightmostnbits(table->depth, h1(key));
		//printf("address in while=%d\n",address);
	}
	printf("here1\n");
	table->buckets[address]->keys[(table->buckets[address]->nkeys)++]=key;
	printf("here2\n");
	printf("table=%p\n",table);
	//printf("nkeys2=%d\n",(table->buckets[address]->nkeys));
	//fprintf(stderr, "not yet implemented\n");
	return true;
}


// lookup whether 'key' is inside 'table'
// returns true if found, false if not
bool xtndbln_hash_table_lookup(XtndblNHashTable *table, int64 key) {
	int i=0,
	 hash=h1(key),
	 address=rightmostnbits(table->depth,hash);
	for(i=0;i<(table->buckets[address]->nkeys);i++)
	{
		if(table->buckets[address]->keys[i]==key)
		{
			return true;
		}
	}
	//fprintf(stderr, "not yet implemented\n");
	return false;
}


// print the contents of 'table' to stdout
void xtndbln_hash_table_print(XtndblNHashTable *table) {
	assert(table);
	printf("--- table size: %d\n", table->size);

	// print header
	printf("  table:               buckets:\n");
	printf("  address | bucketid   bucketid [key]\n");
	//printf("table=%p\n",table);
	// print table and buckets
	int i;
	for (i = 0; i < table->size; i++) {
		// table entry
		printf("%9d | %-9d ", i, table->buckets[i]->id);

		// if this is the first address at which a bucket occurs, print it now
		if (table->buckets[i]->id == i) {
			printf("%9d ", table->buckets[i]->id);

			// print the bucket's contents
			printf("[");
			for(int j = 0; j < table->bucketsize; j++) {
				if (j < table->buckets[i]->nkeys) {
					printf(" %llu", table->buckets[i]->keys[j]);
				} else {
					printf(" -");
				}
			}
			printf(" ]");
		}
		// end the line
		printf("\n");
	}

	printf("--- end table ---\n");
}


// print some statistics about 'table' to stdout
void xtndbln_hash_table_stats(XtndblNHashTable *table) {
	int i=0;
	for(i=0;i<table->size;i++)
	{
		printf("i=%d depth=%d nkeys=%d\n", i,table->buckets[i]->depth, table->buckets[i]->nkeys);
	}
	printf("table->size=%d\n",table->size);
	printf("table->depth=%d\n",table->depth);
	fprintf(stderr, "not yet implemented\n");
}
