#include <sys/types.h>
typedef struct __filestat_t filestat_t;
typedef struct __zip_t zip_t;
typedef struct __zipstream_t zipstream_t;
typedef unsigned char ubyte_t;

void Init();
void* CompressText();
int GetNextPage(filestat_t* fs);
void OpenNextFile(filestat_t* fs);
void ZipPage(zipstream_t* fs);
void PutZip(zipstream_t* zipstream);
void* WriteZip();
void PrintStream(ubyte_t* pStream, size_t szStream);
