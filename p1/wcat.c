#include <stdio.h>
#include <stdlib.h>

#define BufferSize (1024)

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		exit(0);
	}
	FILE* fp;
	char buffer[BufferSize];
	for (int i=1; i < argc; i++)
	{
		if ((fp = fopen(argv[i], "r")) == NULL)
		{
			printf("wcat: cannot open file\n");
			exit(1);
		}
		while (fgets(buffer, BufferSize, fp) != NULL)
		{
			printf("%s", buffer);
		}

		fclose(fp);
	}
	return 0;
}

