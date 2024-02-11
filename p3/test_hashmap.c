#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "hashmap.h"
#include "utilities.h"

int main()
{
	int nLength = 4;
	hashmap_t* pHashMap;
	int nDataLength = 13;
	char** data = Malloc(sizeof(char*) * nDataLength);
	char* szDataOut;
	for (int i=0; i < nDataLength; i++)
	{
		data[i] = Malloc(sizeof(char) * 2);
		data[i][0] = 'a' + i;
		data[i][1] = '\0';
	}
	pHashMap = InitHashMap(nLength);
	for (int i=0; i < nLength; i++)
	{
		PutData(pHashMap, (void*) data[i], i);
	}
	szDataOut = GetData(pHashMap, 0);
	assert(strcmp(szDataOut,"a") == 0);
	free(szDataOut);

	for (int i=nLength; i < 2 * nLength; i++)
	{
		PutData(pHashMap, (void*) data[i], i);
	}
	szDataOut = GetData(pHashMap, 5);
	assert(strcmp(szDataOut, "f") == 0);
	free(szDataOut);

	szDataOut = GetData(pHashMap, 2);
	assert(strcmp(szDataOut, "c") == 0);

	for (int i = 2 * nLength; i < 3 * nLength; i++)
	{
		PutData(pHashMap, (void*) data[i], i);
	}
	szDataOut = GetData(pHashMap, 7);
	assert(strcmp(szDataOut, "h") == 0);
	DestroyHashMap(pHashMap);
	free(data);

	return 0;
}
