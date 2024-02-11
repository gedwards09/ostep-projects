typedef struct __node_t node_t;
typedef struct __node_t
{
	int nSortIndex;
	void* pData;
	node_t* pNext;
} node_t;

void* InitNode(void* pData, int nSortIndex);
void DestroyNode(node_t* pNode);
