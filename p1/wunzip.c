#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BufferSize (1048*1048)
#define WordSize 5
#define InitCharPos 4
#define FlushBuffer() fwrite(outbuf, sizeof(char), outcur - outbuf, stdout); \
			outcur = outbuf

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("wunzip: file1 [file2 ...]\n");
		exit(1);
	}

	size_t len;
	size_t buffersize = sizeof(char) * BufferSize;
	unsigned char* buffer = (unsigned char*) malloc(buffersize);
	if (buffer == NULL)
	{
		printf("error:wunzip.c: unable to allocate buffer memory\n");
		exit(1);
	}
	unsigned char* outbuf = (unsigned char*) malloc(buffersize);
	if (outbuf == NULL)
	{
		printf("error:wunzip.c: unable to allocate buffere memory\n");
		exit(1);
	}
	uint32_t count;
	unsigned char* current;
	unsigned char* outcur = outbuf;
	/* Index of the character symbol mod 5. Initially set to position 4. */
	unsigned short charOffset = InitCharPos;
	FILE* fp;

	while (--argc > 0)
	{
		fp = fopen(*++argv, "r");
		if (argv == NULL)
		{
			printf("error:wunzip.c: unable to open file\n");
			exit(1);
		}
		count = 0;
		charOffset = InitCharPos;
		while ((len = fread(buffer, sizeof(char), buffersize, fp)) > 0)
		{
			current = buffer;
			while (current < buffer + len)
			{
				if ((current - buffer) % WordSize == charOffset)
				{
					//DEBUG
					//printf("count: %u", count);
					while (count-- > 0)
					{
						*outcur++ = *current;
						if (outcur >= outbuf + buffersize)
						{
							FlushBuffer();
						}
					}
				}
				else
				{
					count >>= 8;
					count |= *current << 24;
				}
				current++;
			}
			charOffset = (charOffset + WordSize - (len % WordSize)) % WordSize;
		}
		FlushBuffer();
		fclose(fp);
	}

	free(buffer);
	free(outbuf);
}
