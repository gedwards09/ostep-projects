#include <stdio.h>

#define getchar() getc(stdin)
#define putchar(c) putc((c), stdout)

int main(int argc, char* argv[])
{
	FILE* fp;
	void filecopy(FILE*, FILE*);

	while (--argc > 0)
	{
		if ((fp = fopen(*++argv, "r")) == NULL)
		{
			printf("zcat: can't open %s\n", *argv);
			return 1;
		}
		else
		{
			filecopy(fp, stdout);
			fclose(fp);
		}
	}
	return 0;
}

void filecopy(FILE* ifp, FILE* ofp)
{
	int c;
	while ((c = getc(ifp)) != EOF)
	{
		putc(c, ofp);
	}
}
