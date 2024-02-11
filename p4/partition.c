#include "partition.h"
#include "treemap.h"
#include "utilities.h"

void init_partition(partition_t* pPartition)
{
	PthreadMutexInit(&pPartition->lock);
	init_treemap(&pPartition->treemap);
}

void destroy_partition(partition_t* pPartition)
{
	destroy_treemap(&pPartition->treemap);
}
