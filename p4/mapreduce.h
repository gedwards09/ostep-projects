#ifndef __mapreduce_h__
#define __mapreduce_h__

// Different function pointer types used by MR
typedef char *(*Getter)(char *key, int partition_number);
typedef void (*Mapper)(char *file_name);
typedef void (*Reducer)(char *key, Getter get_func, int partition_number);
typedef unsigned long (*Partitioner)(char *key, int num_partitions);

// External functions: these are what you must define
void MR_Emit(char *key, char *value);

unsigned long MR_DefaultHashPartition(char *key, int num_partitions);

void MR_Run(int argc, char *argv[], 
	    Mapper map, int num_mappers, 
	    Reducer reduce, int num_reducers, 
	    Partitioner partition);

/* struct for passing key-value pairs */
typedef struct __kv_t
{
	char* key;
	char* value;
} kv_t;

void do_produce(int argc, char** argv);
void do_put(char* key, char* value);
void* do_consume(void* args);
void do_get(kv_t* pkv);
void do_put_partition(kv_t* pkv);
void* do_consume_partition(void* arg);
char* get_next_key(char* pKey, int partition_number);
char* get_next(char* key, int partition_number);

#endif // __mapreduce_h__
