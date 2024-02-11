/*
 * Header for utility function declarations
 * @author Greg Edwards
 * @version 1.0
 */
#include <sys/stat.h>
#include <stdlib.h>

void Close(int fd);
void Fstat(int fd, struct stat* fsp);
void* Malloc(size_t size);
void* Mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void* addr, size_t length);
void PthreadCondInit(pthread_cond_t* cond);
void PthreadCondSignal(pthread_cond_t* cond);
void PthreadCondWait(pthread_cond_t* restrict cond, 
	pthread_mutex_t* restrict mutex);
void* PthreadCreate(pthread_t* thread, const pthread_attr_t* att,
	void* (*start_routine)(void*), void *arg);
void PthreadJoin(pthread_t thread, void** retval);
void PthreadMutexInit(pthread_mutex_t* mutex);
void PthreadMutexLock(pthread_mutex_t* mutex);
void PthreadMutexUnlock(pthread_mutex_t* mutex);
int Open(char* file, int flags);
void* Realloc(void* ptr, size_t size);
