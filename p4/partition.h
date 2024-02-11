#ifndef __partition_h__
#define __partition_h__

#include <pthread.h>
#include <stdlib.h>
#include "treemap.h"

typedef struct __partition_t
{
	/* partition lock */
	pthread_mutex_t lock;
	/* treemap for partitoon */
	treemap_t treemap;
} partition_t;

void init_partition(partition_t* pPartition);
void destroy_partition(partition_t* pPartitions);

#endif // __paritition_h__


