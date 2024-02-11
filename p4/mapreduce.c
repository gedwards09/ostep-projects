// DEBUGGING
#include <stdio.h>

#include "mapreduce.h"
#include "partition.h"
#include "treemap.h"
#include "utilities.h"

#define szBuffer (64)

/* bounded buffer of key-value pairs */
kv_t Buffer[szBuffer];
int nFull;
int FillIndex = 0;
int UseIndex = 0;
pthread_mutex_t BufferLock;
pthread_cond_t BufferFill;
pthread_cond_t BufferEmpty;

/* Flag set when finished producing */
int Done = 0;

/* The mapping function passed to MR_Run */
Mapper MapFn;
/* Reduce function passed to MR_Run */
Reducer ReduceFn;
/* Partiton function passed to MR_Run, if present, default partiton otherwise */
Partitioner PartitionFn;
/* the number of partions */
int nPartitions;
/* Partition structures */
partition_t* pPartitions;

/*
 * @fn unsigned long MR_DefaultHashPartition(char* key, int num_partitions)
 * @brief Default partition function
 * @param char* key          [in] Key to hash
 * @param int num_partitions [in] number of partitons
 * @returns hash value of key
 */
unsigned long MR_DefaultHashPartition(char* key, int num_partitions)
{
	unsigned long hash = 5381;
	int c;
	while ((c = *key++) != '\0')
	{
		hash = hash * 33 + c;
	}
	return hash % num_partitions;
}

/*
 * @fn MR_Run(int argc, char* argv[], 
 *            Mapper map, int num_mappers,
 *            Reducer reduce, int num_reducers,
 *            Partitioner partition)
 * @brief Runs distributed map-reduce using user defined Map and Reduce 
 *        operations. 
 *        Keys are generated concurrently and distributed across a number of 
 *        partitions equal to the number of reducers specified. 
 *        Reducer operations only run after all Mappers have completed. 
 *        Key are guaranteed to be processed in sorted order by reducers within
 *        a partition
 * @param int         argc         [in] Command line argument count
 * @param char*       argv[]       [in] The command line arguments
 * @param Mapper      map          [in] Map function point which calls
 *                                      MR_Emit with key-value pairs
 * @param int         num_mappers  [in] Number of mapper threads
 * @param Reducer     reduce       [in] Reduce function pointer to process
 *                                      the values for each emitted key
 * @param int         num_reducers [in] Number of reducer threads. 
 *                                      Determines the number of partitions used
 * @param Partitioner partition    [in] Partition function pointer which maps 
 *                                      keys to a given partition
 */
void MR_Run(int argc, char* argv[], 
			Mapper map, int num_mappers,
			Reducer reduce, int num_reducers,
			Partitioner partition)
{
	pthread_t* pConsumers;

	PthreadMutexInit(&BufferLock);
	PthreadCondInit(&BufferFill);
	PthreadCondInit(&BufferEmpty);

	MapFn = map;
	ReduceFn = reduce;
	PartitionFn = partition != NULL ? partition : MR_DefaultHashPartition;
	nPartitions = num_reducers;

	pPartitions = Malloc(nPartitions * sizeof(partition_t));
	for (int i = 0; i < nPartitions; i++)
	{
		init_partition(&pPartitions[i]);
	}

	pConsumers = Malloc(num_mappers * sizeof(pthread_t));
	for (int i = 0; i < num_mappers; i++)
	{
		PthreadCreate(&pConsumers[i], NULL, do_consume, NULL);
	}

	do_produce(argc, argv);

	Done = 1;
	PthreadCondBroadcast(&BufferFill);

	for (int i = 0; i < num_mappers; i++)
	{
		PthreadJoin(pConsumers[i], NULL);
	}

	pConsumers = Realloc(pConsumers, num_reducers * sizeof(pthread_t));
	for (int i = 0; i < num_reducers; i++)
	{
		int* arg = Malloc(sizeof(int));
		*arg = i;
		PthreadCreate(&pConsumers[i], NULL, do_consume_partition, (void*)arg); 
	}

	for (int i = 0; i < num_reducers; i++)
	{
		PthreadJoin(pConsumers[i], NULL);
	}
	free(pConsumers);

	for (int i = 0; i < nPartitions; i++)
	{
		destroy_partition(&pPartitions[i]);
	}
	free(pPartitions);
}

/*
 * @fn void do_produce(int argc, char** argv)
 * @brief Applies the user specified Mapper to each command line argument
 * @param int    argc [in] Command line argument count
 * @param char** argv [in] The command line arguments
 */
void do_produce(int argc, char** argv)
{
	while (--argc > 0)
	{
		MapFn(*(++argv));
	}
}

/*
 * @fn void MR_Emit(char* key, char* value)
 * @brief Called by user map routine for each key-value pair.
 *        Queues key-value pairs for mapping.
 * @param char* key   [in] Emitted key
 * @param char* value [in] Associated value
 */
void MR_Emit(char* key, char* value)
{
	PthreadMutexLock(&BufferLock);
	while (nFull == szBuffer)
	{
		PthreadCondWait(&BufferEmpty, &BufferLock);
	}
	do_put(key, value);
	PthreadCondSignal(&BufferFill);
	PthreadMutexUnlock(&BufferLock);
}

/*
 * @fn void do_put(char* key, char* value)
 * @brief Pushes the key-value pair to the buffer to be mapped.
 *        Caller must be holding the buffer lock.
 * @param char*   key [in] Emitted key
 * @param char* value [in] Associated value
 */
void do_put(char* key, char* value)
{
	Buffer[FillIndex].key = CopyString(key);
	Buffer[FillIndex].value = CopyString(value);
	FillIndex = (FillIndex + 1) % szBuffer;
	nFull++;
}

/*
 * @fn void* do_consume(void* args)
 * @brief Gets a key value pair and files it to the appropriate partition
 * @param coid* arg [ign] Required, ignored.
 * @returns NULL
 */
void* do_consume(void* args)
{
	kv_t kv;
	while (1)
	{
		PthreadMutexLock(&BufferLock);
		while (nFull == 0 && Done == 0)
		{
			PthreadCondWait(&BufferFill, &BufferLock);
		}
		if (nFull == 0 && Done == 1)
		{
			PthreadMutexUnlock(&BufferLock);
			return NULL;
		}
		do_get(&kv);
		PthreadCondSignal(&BufferEmpty);
		PthreadMutexUnlock(&BufferLock);

		do_put_partition(&kv);
	}
}

/*
 * @fn void do_get(kv_t* pkv)
 * @brief Get the next key-value pair on the queue
 *        Caller must be holding the buffer lock
 * @param kv_t* pkv [out] Struct holding the key-value pair
 */
void do_get(kv_t* pkv)
{
	*pkv = Buffer[UseIndex];
	Buffer[UseIndex].key = NULL;
	Buffer[UseIndex].value = NULL;
	UseIndex = (UseIndex + 1) % szBuffer;
	nFull--;
}

/*
 * @fn void do_put_partition(kv_t* pkv)
 * @brief Adds the key-value pair to the appropriate partition
 * @param kv_t* pkv [in] Struct holding the key-value pair
 */
void do_put_partition(kv_t* pkv)
{
	int index = PartitionFn(pkv->key, nPartitions);
	partition_t* pPartition = &pPartitions[index];
	PthreadMutexLock(&pPartition->lock);
	treemap_add(&pPartition->treemap, pkv->key, pkv->value);
	PthreadMutexUnlock(&pPartition->lock);
	free(pkv->key);
	free(pkv->value);
}

/*
 * @fn void* do_consume_partition(coid* arg)
 * @brief Loops over all keys in the partition data structer and applies the 
 *        users supplied Reducer function to all values associated with the key.
 * @param void* arg [in] Pointer to integer partition number.
 *                       Pointer is freed after use
 * @returns NULL
 */
void* do_consume_partition(void* arg)
{
	int partition_number = *(int*)arg;
	free(arg);
	char* pKey = NULL;
	while ((pKey = get_next_key(pKey, partition_number)) != NULL)
	{
		ReduceFn(pKey, get_next, partition_number);
	}
	return NULL;
}

/*
 * @fn char* get_next_key(char* pKey, int partition_number)
 * @brief gets the next key following pKey for the specified partition
 * @param char* pKey             [in] The input key to get next key in order.
 *                                    Pass NULL to get the first key. 
 * @param int   partition_number [in] The parition being iterated over.
 * @returns The next key following the input key.
 */
char* get_next_key(char* pKey, int partition_number)
{
	return treemap_get_next_key(&pPartitions[partition_number].treemap, pKey);
}

/* 
 * @fn char* get_next(char* pKey, int partition_number)
 * @brief gets the next value for key pKey in the specified partition
 * @param char* pKey             [in] The key to iterate values.
 * @param int   partition_number [in] The parition of the key.
 * @returns The next value for the given key.
 */
char* get_next(char* pKey, int partition_number)
{
	if (pKey == NULL)
	{
		return NULL;
	}

	return treemap_get_next_value(&pPartitions[partition_number].treemap, pKey);
}
