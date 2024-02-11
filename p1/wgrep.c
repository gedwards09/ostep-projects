#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BufferSize (1024)

int* FailureTable = NULL;

int* getFailureTable(char* pattern);
int wsearch(char* text, char* pattern, ssize_t len);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("wgrep: searchterm [file ...]\n");
		exit(1);
	}

	char* pattern = argv[1];
	FailureTable = getFailureTable(pattern);
	if (FailureTable == NULL)
	{
		printf("error: wgrep: failed to allocate memory for FailureTable\n");
		exit(1);
	}
	FILE* fp;
	size_t bufsize = sizeof(char) * BufferSize;
	ssize_t len;
	char* line = (char*) malloc(bufsize);
	if (line == NULL)
	{
		printf("error: wgrep: failed to allocate memory for buffer\n");
		exit(1);
	}
	int i = argc > 2 ? 2 : 1;
	while (i < argc)
	{
		if (argc == 2)
		{
			fp = stdin;
			i++;
		}
		else if ((fp = fopen(argv[i++], "r")) == NULL)
		{
			printf("wgrep: cannot open file\n");
			free(line);
			exit(1);
		}
		while ((len = getline(&line, &bufsize, fp)) != -1)
		{
			if (wsearch(line, pattern, len))
			{
				printf("%s", line);
			}
		}

		fclose(fp);
	}

	free(FailureTable);
	free(line);

	return 0;
}

/*
 * Implements KMP string search algorithm
 * @param text Search text string
 * @param pattern String pattern to search for
 * @param textlen Length of search text
 * @returns true if search string found
 */
int wsearch(char* text, char* pattern, ssize_t textlen)
{
	int m = strlen(pattern);
	int j = 0;
	int k = 0;
	while (k < textlen)
	{
		if (pattern[j] == text[k])
		{
			if (j == m - 1)
			{
				// string found
				return 1;
			}
			else
			{
				j++;
				k++;
			}
		}
		else if (j == 0)
		{
			k++;
		}
		else
		{
			j = FailureTable[j-1];
		}
	}

	//string not found
	return 0;
}

/*
 * Allocates array the same length as pattern and populates failure array table.
 * Caller must free after use.
 * @param pattern The string pattern being searched
 * @returns Failure table for the given search string
 */
int* getFailureTable(char* pattern)
{
	int m = strlen(pattern);
	int* failureTable = (int*) malloc(m * sizeof(int));
	if (failureTable == NULL)
	{
		exit(1);
	}
	int i = 0;
	int j = 1;
	while (j < m)
	{
		if (pattern[i] == pattern[j])
		{
			failureTable[j] = i + 1;
			i++;
			j++;
		}
		else if (i == 0)
		{
			j++;
		}
		else
		{
			i = failureTable[i-1];
		}
	}

	return failureTable;
}

