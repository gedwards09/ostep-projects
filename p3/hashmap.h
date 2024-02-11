#include <pthread.h>
#include <sys/types.h>

typedef struct __hashmap_t
{
	size_t nLength;
	node_t** pNodes;
	pthread_mutex_t* pLocks;
	pthread_cond_t* pConds;
} hashmap_t;

hashmap_t* InitHashMap(size_t nLength);
void PutData(hashmap_t* pHashMap, void* pData, int nSortIndex);
void PutNode(hashmap_t* pHashMap, node_t* pNode);
void* GetData(hashmap_t* pHashMap, int nSortIndex);
void* Remove(hashmap_t* pHashMap, int nSortIndex);
void DestroyHashMap(hashmap_t* pHashMap);
void DestroyData(hashmap_t* pHashMap);
void ToString(hashmap_t* pHashMape);
