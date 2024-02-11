/**
 * Wrapper functions for system and C library functions
 *
 * @author: Greg Edwards
 * @version: 1.0
 */

#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void Close(int fd)
{
	int out;
	out = close(fd);
	if (out < 0)
	{
		printf("utilities.c:close:could not close file\n");
		exit(1);
	}
}

void Fstat(int fd, struct stat* fsp)
{
	if (fstat(fd, fsp) == -1)
	{
		printf("utilities.c:fstat:cannot get file statistics\n");
		exit(1);
	}
}

void* Malloc(size_t size)
{
	void* out;
	out = malloc(size);
	if (out == NULL)
	{
		printf("utilities.c:malloc:could not allocate %ld bytes.\n", size);
		exit(1);
	}

	return out;
}

void* Mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void* memp;
	memp = mmap(addr, len, prot, flags, fd, offset);
	if (memp == MAP_FAILED)
	{
		printf("utilities.c:mmap:memory map failed\n");
		exit(1);
	}

	return memp;
}

void Munmap(void* addr, size_t length)
{
	if (munmap(addr, length) != 0)
	{
		printf("utilities.c:munmap:Unable to unmap memory\n");
		exit(1);
	}
}


/*
 * Wrapper for open() syscall
 */
int Open(char* file, int flags)
{
	int fd;
	fd = open(file, flags);
	if (fd < 0)
	{
		printf("utilities.c:open:Cannot open file\n");
		exit(1);
	}

	return fd;
}

void PthreadCondInit(pthread_cond_t* cond)
{
	int out;
	out = pthread_cond_init(cond, NULL);
	if (out != 0)
	{
		printf("utilities.c:pthread_cond_init:"
			"failed to initialize condition\n");
		exit(1);
	}
}

void PthreadCondSignal(pthread_cond_t* cond)
{
	int out;
	out = pthread_cond_signal(cond);
	if (out != 0)
	{
		printf("utilities.c:pthread_cond_signal:conditional signal failed\n");
		exit(1);
	}
}

void PthreadCondWait(pthread_cond_t* restrict cond, 
					pthread_mutex_t* restrict mutex)
{
	int out;
	out = pthread_cond_wait(cond, mutex);
	if (out != 0)
	{
		printf("utilities.c:pthread_cond_wait:conditional wait failed\n");
		exit(1);
	}
}

void PthreadCreate(pthread_t* thread, const pthread_attr_t* attr,
                  void* (*start_routine)(void*), void* arg)
{
	int out;
	out = pthread_create(thread, attr, start_routine, arg);
	if (out != 0)
	{
		printf("utilities.c:pthread_create:Unable to create pthread\n");
		exit(1);
	}
}

void PthreadJoin(pthread_t thread, void** retval)
{
	int out;
	out = pthread_join(thread, retval);
	if (out != 0)
	{
		printf("utilities.c:pthread_join:Unable to join pthread\n");
		exit(1);
	}
}

void PthreadMutexInit(pthread_mutex_t* mutex)
{
	int out;
	out = pthread_mutex_init(mutex, NULL);
	if (out != 0)
	{
		printf("utilities.c:pthread_mutex_init:"
			"failed to initialize mutex lock\n");
		exit(1);
	}
}

void PthreadMutexLock(pthread_mutex_t* mutex)
{
	int out;
	out = pthread_mutex_lock(mutex);
	if (out != 0)
	{
		printf("utilities.c:pthread_mutex_lock:Unable to perform mutex lock\n");
		exit(1);
	}
}

void PthreadMutexUnlock(pthread_mutex_t* mutex)
{
	int out;
	out = pthread_mutex_unlock(mutex);
	if (out != 0)
	{
		printf("utilities.c:pthread_mutex_unlock:mutex unlock failed\n");
		exit(1);
	}
}

void* Realloc(void* ptr, size_t size)
{
	void* out;
	out = realloc(ptr, size);
	if (out == NULL)
	{
		printf("utilities.c:realloc:memory allocation failed\n");
		exit(1);
	}

	return out;
}
