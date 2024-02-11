#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include "hashmap.h"
#include "utilities.h"

hashmap_t* InitHashMap(size_t nLength)
{
	hashmap_t* pHashMap = Malloc(sizeof(hashmap_t));
	pHashMap->nLength = nLength;
	pHashMap->pNodes = Malloc(sizeof(node_t*) * nLength);
	pHashMap->pLocks = Malloc(sizeof(pthread_mutex_t) * nLength);
	pHashMap->pConds = Malloc(sizeof(pthread_cond_t) * nLength);
	for (int i=0; i < nLength; i++)
	{
		pHashMap->pNodes[i] = NULL;
		PthreadMutexInit(&pHashMap->pLocks[i]);
		PthreadCondInit(&pHashMap->pConds[i]);
	}

	return pHashMap;
}

void PutData(hashmap_t* pHashMap, void* pData, int nSortIndex)
{
	node_t* pNode = InitNode(pData, nSortIndex);
	PutNode(pHashMap, pNode);
}

void PutNode(hashmap_t* pHashMap, node_t* pNode)
{
	int nIndex;
	node_t* pCurrentNode;

	nIndex = pNode->nSortIndex % pHashMap->nLength;
	PthreadMutexLock(&pHashMap->pLocks[nIndex]);
	pCurrentNode = pHashMap->pNodes[nIndex];
	if (pCurrentNode == NULL)
	{
		pHashMap->pNodes[nIndex] = pNode;
	}
	else if (pNode->nSortIndex < pCurrentNode->nSortIndex)
	{
		pNode->pNext = pCurrentNode;
		pHashMap->pNodes[nIndex] = pNode;
	}
	else
	{
		while (pCurrentNode->pNext != NULL 
				&& pCurrentNode->pNext->nSortIndex <= pNode->nSortIndex)
		{
			pCurrentNode = pCurrentNode->pNext;
		}
		pNode->pNext = pCurrentNode->pNext;
		pCurrentNode->pNext = pNode;
	}
	PthreadCondSignal(&pHashMap->pConds[nIndex]);
	PthreadMutexUnlock(&pHashMap->pLocks[nIndex]);
}

void* GetData(hashmap_t* pHashMap, int nSortIndex)
{
	int nIndex;
	void* pData;

	nIndex = nSortIndex % pHashMap->nLength;
	PthreadMutexLock(&pHashMap->pLocks[nIndex]);
	while ((pData = Remove(pHashMap, nSortIndex)) == NULL)
	{
		PthreadCondWait(&pHashMap->pConds[nIndex], &pHashMap->pLocks[nIndex]);
	}
	PthreadMutexUnlock(&pHashMap->pLocks[nIndex]);
	return pData;
}

void* Remove(hashmap_t* pHashMap, int nSortIndex)
{
	int nIndex;
	node_t* pCurrentNode;
	node_t* pOldNode;
	void* pData;

	nIndex = nSortIndex % pHashMap->nLength;
	pCurrentNode = pHashMap->pNodes[nIndex];
	if (pCurrentNode == NULL)
	{
		return NULL;
	}
	else if (pCurrentNode->nSortIndex == nSortIndex)
	{
		pHashMap->pNodes[nIndex] = pCurrentNode->pNext;
		pData = pCurrentNode->pData;
		DestroyNode(pCurrentNode);
	}
	else if (pCurrentNode->pNext == NULL 
				|| pCurrentNode->pNext->nSortIndex > nSortIndex)
	{
		return NULL;
	}
	else
	{
		while (pCurrentNode->pNext->pNext != NULL
				&& pCurrentNode->pNext->pNext->nSortIndex <= nSortIndex)
		{
			pCurrentNode = pCurrentNode->pNext;
		}
		if (pCurrentNode->pNext->nSortIndex != nSortIndex)
		{
			return NULL;
		}
		pData = pCurrentNode->pNext->pData;
		pOldNode = pCurrentNode->pNext;
		pCurrentNode->pNext = pCurrentNode->pNext->pNext;
		DestroyNode(pOldNode);
	}

	return pData;
}

void DestroyHashMap(hashmap_t* pHashMap)
{
	node_t* pCurrentNode;
	node_t* pNextNode;
	for (int i = 0; i < pHashMap->nLength; i++)
	{
		pCurrentNode = pHashMap->pNodes[i];
		while (pCurrentNode != NULL)
		{
			pNextNode = pCurrentNode->pNext;
			free(pCurrentNode->pData);
			DestroyNode(pCurrentNode);
			pCurrentNode = pNextNode;
		}
	}
	free(pHashMap->pNodes);
	free(pHashMap->pLocks);
	free(pHashMap->pConds);
	free(pHashMap);
}

void DestroyData(hashmap_t* pHashMap)
{
	node_t* pCurrentNode;
	for (int i = 0; i < pHashMap->nLength; i++)
	{
		pCurrentNode = pHashMap->pNodes[i];
		while (pCurrentNode != NULL)
		{
			free(pCurrentNode->pData);
			pCurrentNode->pData = NULL;
			pCurrentNode = pCurrentNode->pNext;
		}
	}
}

void ToString(hashmap_t* pHashMap)
{
	node_t* pCurrentNode;

	for (int i = 0; i < pHashMap->nLength; i++)
	{
		pCurrentNode = pHashMap->pNodes[i];
		printf("H->");
		while (pCurrentNode != NULL)
		{
			printf("%d->", pCurrentNode->nSortIndex);
			pCurrentNode = pCurrentNode->pNext;
		}
		printf("NULL\n");
	}
}
