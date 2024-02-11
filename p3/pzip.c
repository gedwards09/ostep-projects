#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "node.h"
#include "hashmap.h"
#include "pzip.h"
#include "utilities.h"

#define szTextBuffer (4096)
#define szRleBytes (5)
#define szZipBuffer (512)
#define szHashMap (16)
#define Shift (8)
#define MaxOpenFiles (8)
#define szWriteBuffer (512)

typedef unsigned char ubyte_t;

typedef struct __zip_t
{
	unsigned int count;
	unsigned char c;
} zip_t;

typedef struct __args_t
{
	int argc;
	char** argv;
} args_t;

typedef struct __filestat_t
{
	int index;
	int fd;
	size_t size;
	off_t offset;
} filestat_t;

typedef struct __zipstream_t
{
	int valid;
	int index;
	int fd;
	size_t size;
	char* text;
	/* number of run-length endcodings packed */
	size_t streamsize;
	ubyte_t* stream;
} zipstream_t;

args_t mArgs;

int mVerbose = 0;

filestat_t mCurrentFile;
pthread_mutex_t mCurrentFileLock;

hashmap_t* mpZipHash;

// handle tracking and closing files later

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("pzip: file1 [file2 ...]\n");
		exit(1);
	}

	if (strcmp(argv[1], "-v") == 0)
	{
		mVerbose = 1;
		argc--;
		argv++;
	}

	mArgs.argc = argc;
	mArgs.argv = argv;

	Init();

	pthread_t fileThread1;
	pthread_t fileThread2;
	pthread_t writeThread;

	PthreadCreate(&fileThread2, NULL, CompressText, NULL);
	PthreadCreate(&fileThread1, NULL, CompressText, NULL);
	PthreadCreate(&writeThread, NULL, WriteZip, NULL);

	PthreadJoin(fileThread1, NULL);	
	PthreadJoin(fileThread2, NULL);
	PthreadJoin(writeThread, NULL);

	DestroyData(mpZipHash);
	DestroyHashMap(mpZipHash);

	return 0;
}

void Init()
{
	mCurrentFile.size = 0;
	mCurrentFile.offset = 0;
	PthreadMutexInit(&mCurrentFileLock);

	mpZipHash = InitHashMap(szHashMap);
}

void* CompressText()
{
	int rc;
	filestat_t fs;
	zipstream_t* zs;

	while (1)
	{
		rc = GetNextPage(&fs);
		zs = Malloc(sizeof(zipstream_t));
		if (rc != 0)
		{
			zs->valid = 1;
			zs->index = fs.index;
			zs->stream = NULL;
			PutZip(zs);
			return NULL;
		}
		zs->index = fs.index;
		zs->fd = fs.fd;
		zs->size = fs.size;
		zs->text = Mmap(NULL, fs.size, PROT_READ, 
						MAP_PRIVATE, fs.fd, fs.offset);
		zs->valid = 1;
		ZipPage(zs);
		PutZip(zs);
	}
}

int GetNextPage(filestat_t* fs)
{
	filestat_t* current;

	PthreadMutexLock(&mCurrentFileLock);
	current = &mCurrentFile;
	fs->index = (current->index)++;
	if (current->size == current->offset)
	{	
		if (--mArgs.argc > 0)
		{
			OpenNextFile(current);
		}
		else
		{
			// done reading files
			PthreadMutexUnlock(&mCurrentFileLock);
			return -1;
		}
	}
	fs->fd = current->fd;
	fs->offset = current->offset;
	if (current->size - current->offset > szTextBuffer)
	{
		fs->size = szTextBuffer;
	}
	else
	{
		fs->size = current->size - current->offset;
	}
	current->offset += fs->size;
	PthreadMutexUnlock(&mCurrentFileLock);

	return 0;
}

void OpenNextFile(filestat_t* fs)
{
	struct stat s;

	fs->fd = Open(*(++mArgs.argv), O_RDONLY);
	Fstat(fs->fd, &s);
	fs->size = s.st_size;
	fs->offset = 0;
}

void ZipPage(zipstream_t* fs)
{
	char* pChar = fs->text;
	size_t szBuffer = szRleBytes * szZipBuffer;
	ubyte_t* pBuffer = Malloc(sizeof(ubyte_t) * szBuffer);
	ubyte_t* pCurrentByte = pBuffer;
	zip_t zip;
	size_t szStream = 0;

	while (pChar < fs->text + fs->size)
	{
		zip.count = 0;
		zip.c = *pChar;
		while (pChar < fs->text + fs->size && zip.c == *pChar)
		{
			zip.count++;
			pChar++;
		}
		for (int i = 0; i < sizeof(int); i++)
		{
			*pCurrentByte = zip.count >> (i * Shift);
			pCurrentByte++;
		}
		*pCurrentByte = zip.c;
		pCurrentByte++;
		szStream++;
		if (szRleBytes * szStream >= szBuffer)
		{
			szBuffer *= 2;
			pBuffer = Realloc(pBuffer, sizeof(ubyte_t) * szBuffer);
			pCurrentByte = pBuffer + szRleBytes * szStream;
		}
	}

	fs->stream = pBuffer;
	fs->streamsize = szStream;
	Munmap(fs->text, fs->size);
	fs->text = NULL;
	fs->size = 0;
}

void PutZip(zipstream_t* zipstream)
{
	PutData(mpZipHash, (void*) zipstream, zipstream->index);
	if (mVerbose)
	{
		printf("put index: %d (%d)\n",  
				zipstream->index % szHashMap, 
				zipstream->index / szHashMap);
		printf("\tstreamsize: 0x%lx\n", zipstream->streamsize);
	}
}

void* WriteZip()
{
	size_t szBuffer = szRleBytes * szWriteBuffer;
	ubyte_t* pBuffer;
	ubyte_t* pCurrent;
	size_t szStream;
	zipstream_t* pZipStream;
	ubyte_t* pStream;
	int index;
	unsigned int ctPrev;
	unsigned int ctCur;

	pBuffer = Malloc(sizeof(ubyte_t) * szBuffer);
	pCurrent = pBuffer;
	szStream = 0;

	index = 0;
	while (1)
	{
		pZipStream = (zipstream_t*) GetData(mpZipHash, index);

		if (mVerbose == 1)
		{
			printf("GOT INDEX: %d\n", pZipStream->index % szHashMap);
			PrintStream(pZipStream->stream, pZipStream->streamsize);
		}

		pStream = pZipStream->stream;

		if (pStream == NULL)
		{
			if (szStream > 0)
			{
				fwrite(pBuffer, sizeof(ubyte_t), szStream, stdout);
			}
			free(pZipStream);
			free(pBuffer);
			break;
		}

		//peek previous char
		if (pCurrent > pBuffer && pStream[4] == *(pCurrent - 1))
		{
			// unpack uint
			pCurrent--;
			ctPrev = 0;
			ctCur = 0;
			for (int i=0; i < sizeof(int); i++)
			{
				pCurrent--;
				ctPrev = (ctPrev << Shift) + *pCurrent;
				ctCur = (ctCur << Shift) + pStream[4 - i - 1];
			}
			//add current count to previous count
			ctCur += ctPrev;
			for (int i=0; i < sizeof(int); i++)
			{
				*pCurrent = ctCur >> (i * Shift);
				pCurrent++;
			}
			pCurrent++;
			pStream += 5;
		}

		// flush current buffer
		if (szStream > 0
				&& szStream + szRleBytes * pZipStream->streamsize > szBuffer)
		{
			fwrite(pBuffer, sizeof(ubyte_t), szStream, stdout);
			szStream = 0;
			pCurrent = pBuffer;
		}

		// dynamic resize buffer
		if (szRleBytes * pZipStream->streamsize > szBuffer)
		{
			free(pBuffer);
			szBuffer = szRleBytes * pZipStream->streamsize;
			pBuffer = Malloc(sizeof(ubyte_t) * szBuffer);
			pCurrent = pBuffer;
		}

		//copy to buffer
		while (pStream < pZipStream->stream 
							+ szRleBytes * pZipStream->streamsize)
		{
			*pCurrent = *pStream;
			pStream++;
			pCurrent++;
			szStream++;
		}

		free(pZipStream->stream);
		free(pZipStream);
		index++;
	}

	return NULL;
}

void PrintStream(ubyte_t* pStream, size_t szStream)
{
	unsigned int ct;
	for (int i=0; i < szStream; i++)
	{
		ct = 0;
		for (int j=0; j < sizeof(int); j++)
		{
			ct += (*pStream) << (j * Shift);
			pStream++;
		}
		printf("\t0x%x: %x\n", ct, *pStream);
		pStream++;
	}
}
