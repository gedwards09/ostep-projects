#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"
#include "utilities.h"

void map(treemap_t* pTreeMap, char* filename);

int main(int argc, char** argv)
{
	treemap_t treeMap;
	init_treemap(&treeMap);

	if (argc < 2)
	{
		exit(1);
	}

	while (--argc > 0)
	{
		map(&treeMap, *(++argv));
	}

	printf("root: %s\n", treeMap.root->key);
	print_treemap(&treeMap);

	destroy_treemap(&treeMap);

	return 0;
}

void map(treemap_t* pTreeMap, char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("test_treemap.c:map:unable to open file %s\n", filename);
		exit(1);
	}

	char* line = NULL;
	size_t size = 0;
	char* token;
	char* dummy;
	while (getline(&line, &size, fp) != -1)
	{
		dummy = line;
		while((token = strsep(&dummy, " \t\n\r")) != NULL)
		{
			if (strlen(token) > 0)
			{
				treemap_add(pTreeMap, CopyString(token), CopyString("1"));
			}
		}
	}
	free(line);
	fclose(fp);
}

