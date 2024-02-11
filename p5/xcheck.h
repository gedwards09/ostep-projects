
#ifndef __xcheck_h__
#define __xcheck_h__

#include "types.h"
#include "param.h"
#include "fs.h"

#ifndef T_DIR
#define T_DIR  1   // Directory
#endif
#ifndef T_FILE
#define T_FILE 2   // File
#endif
#ifndef T_DEV
#define T_DEV  3   // Device
#endif

#define D_DOT (1)
#define D_DOTDOT (2) 

#define SETBIT(Block, B) (Block[B/8] |= (1 << B % 8)) 
#define GETBIT(Block, B) (Block[B/8] & (1 << B % 8))

#define ImageNotFound "image not found."
#define UsageError "Usage: xcheck <file_system_image>"
#define RootDirectoryError "ERROR: root directory does not exist."
#define InvalidInode "ERROR: bad inode."
#define DirectoryLinkError\
	"ERROR: directory appears more than once in file system."
#define InodeNotInUse\
	"ERROR: inode marked use but not found in a directory." // [sic]
#define ReferenceToFreeInode\
	"ERROR: inode referred to in directory but marked free."
#define InvalidDirectAddress "ERROR: bad direct address in inode."
#define InvalidIndirectAddress "ERROR: bad indirect address in inode."
#define FreeBlockInUse\
	"ERROR: address used by inode but marked free in bitmap."
#define DuplicateDirectAddressReference\
	"ERROR: direct address used more than once."
#define DuplicateIndirectAddressReference\
	"ERROR: indirect address used more than once."
#define DirectoryFormatError "ERROR: directory not properly formatted."
#define AllocatedBlockNotUsed\
	"ERROR: bitmap marks block in use but it is not in use."
#define TooManyDirectoryLinks\
	"ERROR: directory appears more than once in file system."
#define BadReferenceCount\
	"ERROR: bad reference count for file."

typedef struct __idata_t
{
	uint type;
	uint nlink;
	uint ncount;
	uint parent;
} idata_t;

typedef struct __fs_t
{
	uint szdata;
	void* data;
	uint szbblock;
	uchar* bblock;
	uint szidata;
	idata_t* idata;
} fs_t;

void breadsb(fs_t* pfs, struct superblock* sb);
void raise_exception(char* msg);
void xcheck_validate(fs_t* pfs);
void* bread(fs_t* pfs, uint nb);
void do_validate_inode_type(struct dinode* pi);
void do_validate_inode_data(fs_t* pfs, struct dinode* pi, ushort inum);
uint get_and_validate_data_block_addr(fs_t* pfs, struct dinode* pi, int nd);
void do_validate_direct_addr(fs_t* pfs, uint addr);
int data_addr_is_valid(fs_t* pfs, uint addr);
int data_block_reference_is_unique(fs_t* pfs, uint addr);
void do_validate_data_block_in_use(fs_t* pfs, uint addr);
void do_validate_indirect_addr(fs_t* pfs, uint addr);
void update_idata(fs_t* pfs, int childinum, int parentinum);
#endif // __xcheck_h__
