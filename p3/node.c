#include <stdlib.h>
#include "node.h"
#include "utilities.h"

void* InitNode(void* pData, int nSortIndex)
{
	node_t* pNode = Malloc(sizeof(node_t));
	pNode->nSortIndex = nSortIndex;
	pNode->pData = pData;
	pNode->pNext = NULL;

	return pNode;
}

void DestroyNode(node_t* pNode)
{
	free(pNode);
}
