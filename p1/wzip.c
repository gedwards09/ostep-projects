#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RunLengthBytes 5
#define BufferSize (1024*1024)
#define RunLengthBufferSize (256*256)
#define flushBuffer() fwrite(outbuffer, sizeof(rle_t), idx, stdout); idx = 0
#define cpyToBuf(n, c)  outbuffer[idx].byte[0] = (n) & 0xFF; \
						outbuffer[idx].byte[1] = ((n) >> 8) & 0xFF; \
						outbuffer[idx].byte[2] = ((n) >> 16) & 0xFF; \
						outbuffer[idx].byte[3] = ((n) >> 24) & 0xFF; \
						outbuffer[idx++].byte[4] = (c)

typedef unsigned char byte_t;

typedef struct rle
{
	byte_t byte[RunLengthBytes];
} rle_t;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("wzip: file1 [file2 ...]\n");
		exit(1);
	}

	FILE* fp;
	char* cur;
	char* ptr;
	char* tail;
	size_t buffsize = sizeof(char) * BufferSize;
	char* inbuffer = (char*) malloc(buffsize);
	if (inbuffer == NULL)
	{
		printf("wzip: cannot allocate buffer space\n");
		exit(1);
	}
	size_t idx;
	rle_t* outbuffer = (rle_t*)malloc(sizeof(rle_t) * RunLengthBufferSize);
	if (outbuffer == NULL)
	{
		printf("wzip: cannot allocate buffer space\n");
		exit(1);
	}
	byte_t curchar;
	uint32_t counter = 0;
	ssize_t len;

	while (--argc > 0)
	{
		if ((fp = fopen(*++argv, "r")) == NULL)
		{
			printf("wzip: cannot open file\n");
			exit(1);
		}

		while ((len = getdelim(&inbuffer, &buffsize, EOF, fp)) != -1)
		{
			cur = inbuffer;
			ptr = inbuffer;
			tail = inbuffer + len;
			
			// if leftover from previous iteration needs to be copied
			if (counter && *cur != curchar)
			{
				cpyToBuf(counter, curchar);
				counter = 0;
			}

			while (ptr < tail)
			{
				while (*cur == *ptr && ++ptr < tail) {  }
				
				if (ptr < tail)
				{
					if (idx >= RunLengthBufferSize)
					{
						flushBuffer();
					}

					//write RLE to outbuffer
					if (counter)
					{
						counter += ptr - cur;
						cpyToBuf(counter, curchar);
						counter = 0;
					}
					else
					{
						cpyToBuf(ptr-cur, *cur);
					}
					cur = ptr;
				}
				else
				{
					//save off state
					counter += ptr - cur;
					curchar = (byte_t) *cur;
				}
			}
		}

		fclose(fp);
	}

	if (counter)
	{
		cpyToBuf(counter, curchar);
		flushBuffer();
	}

	free(inbuffer);
	free(outbuffer);

	return 0;
}
