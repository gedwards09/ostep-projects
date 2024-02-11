#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "xcheck.h"

int nbitmap = FSSIZE/(BSIZE*8) + 1;
int ninodeblocks;
int nmeta;
struct superblock sb;

int main(int argc, char* argv[])
{
	int fsfd;
	fs_t fs;

	if (argc != 2)
	{
		raise_exception(UsageError);
	}

	fsfd = open(argv[1], O_RDONLY); 
	if (fsfd < 0)
	{
		if (errno == ENOENT)
		{
			fprintf(stderr, "image not found.\n");
		}
		else
		{
			printf("xcheck.c:could not open file %s.\n", argv[1]);
		}
		exit(1);
	}
	fs.szdata = FSSIZE * BSIZE;
	fs.data = mmap(NULL, fs.szdata, PROT_READ, MAP_PRIVATE, fsfd, 0);
	if (fs.data < 0)
	{
		printf("xcheck.c:mmap:file read error.\n");
		exit(1);
	}

	breadsb(&fs, &sb);
	ninodeblocks = sb.ninodes / IPB + 1;
	nmeta = 2 + sb.nlog + ninodeblocks + nbitmap;
	
	fs.szbblock = nbitmap * BSIZE;
	fs.bblock = calloc(fs.szbblock, sizeof(uchar));
	if (fs.bblock == NULL)
	{
		printf("xcheck.c:Unable to allocate bblock memory\n");
		exit(1);
	}
	fs.szidata = sb.ninodes + 1;
	fs.idata = calloc(fs.szidata, sizeof(idata_t));
	if (fs.idata == NULL)
	{
		printf("xcheck.c:unable to allocate idata memory\n");
		exit(1);
	}

	xcheck_validate(&fs);

	free(fs.bblock);
	free(fs.idata);
	munmap(fs.data, fs.szdata);
	close(fsfd);

	return 0;

}

void breadsb(fs_t* pfs, struct superblock* sb)
{
	void* bp;
	bp = bread(pfs, 1);
	*sb = *(struct superblock*)bp;
}

void* bread(fs_t* pfs, uint nb)
{
	return pfs->data + nb * BSIZE;
}

void raise_exception(char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void print_inode(struct dinode* pdi)
{
	printf("type %hi\nnlink %hi\nsize %u\n", pdi->type, pdi->nlink, pdi->size);
}

void print_dir(struct dirent* pdirent)
{
	for (int i = 0; i < BSIZE / sizeof(struct dirent); i++)
	{
		printf("(%hi) -> %s\n", pdirent->inum, pdirent->name);
	}
}

void print_bitmap(void* bblock, uint sz)
{
	void* ptr;
	ptr = bblock;
	for (int i=0; i < sz; i++, ptr++)
	{
		if (i % 8 == 0)
		{
			printf(" ");
			if (i % 16 == 0)
			{
				printf("\n");
			}
		}
		printf("%x ", *(unsigned char*)ptr);
	}
	printf("\n");
}

void print_idata(fs_t* pfs)
{
	idata_t idata;
	for (int inum = 0; inum < pfs->szidata; inum++)
	{
		idata = pfs->idata[inum];
		printf("%d -> %d (%d)\n\texp: %d act:%d\n", 
			inum, idata.parent, idata.type, idata.nlink, idata.ncount); 
	}
}

void xcheck_validate(fs_t* pfs)
{
	int nb;
	void* pb;
	struct dinode* pi;
	int nib;
	int offset;
	int inum;
	int nbb;
	uchar* pexp;
	uchar* pact;
	idata_t* pidata;

	// mark boot block, superblock, and log blocks in use
	for (nb = 0; nb < 2 + sb.nlog; nb++)
	{
		SETBIT(pfs->bblock, nb);
	}

	// walk inode blocks
	for (nib = 0; nib < ninodeblocks; nb++, nib++)
	{
		SETBIT(pfs->bblock, nb);
		pb = bread(pfs, nb);
		for (offset = 0, inum = nib * IPB; 
			offset < IPB && inum <= sb.ninodes; 
			offset++, inum++)
		{
			pi = (struct dinode*)pb + offset;
			do_validate_inode_type(pi);

			pfs->idata[inum].type = pi->type;
			pfs->idata[inum].nlink = pi->nlink;
			// always validate root even if type data is corrupted
			if (inum != ROOTINO && (pi->type == 0 || pi->type == T_DEV))
			{
				continue;
			}

			do_validate_inode_data(pfs, pi, inum);
		}
	}
	
	// mark bitmap block in use
	for (nbb = 0; nbb < nbitmap; nbb++, nb++)
	{
		SETBIT(pfs->bblock, nb);
	}

	// validate bitmaps are equal
	pexp = (uchar*)bread(pfs, BBLOCK(0, sb));
	pact = (uchar*)pfs->bblock;
	for (int i = 0; i < pfs->szbblock; i++, pexp++, pact++)
	{
		if ((*pexp & ~*pact) != 0)
		{
			// bit marked used but not referenced
			raise_exception(AllocatedBlockNotUsed);
		}
	}

	// validate reference ncounts
	for (inum = 0; inum <= sb.ninodes; inum++)
	{
		pidata = &pfs->idata[inum];
		if (pidata->type == T_DIR)
		{
			if (pidata->ncount == 0)
			{
				raise_exception(InodeNotInUse);
			}
			else if (pidata->ncount > 1)
			{
				raise_exception(TooManyDirectoryLinks);
			}
		}
		else if (pidata->type == T_FILE)
		{
			if (pidata->ncount == 0)
			{
				raise_exception(InodeNotInUse);
			}
			else if (pidata->ncount != pidata->nlink)
			{
				raise_exception(BadReferenceCount);
			}
		}
		else if (pidata->type == 0 && pidata->ncount > 0)
		{
			raise_exception(ReferenceToFreeInode);
		}
	}
}

void do_validate_inode_type(struct dinode* pi)
{
	switch(pi->type)
	{
		case 0:
		case T_DEV:
		case T_DIR:
		case T_FILE:
			return;
		default:
			raise_exception(InvalidInode);
	}
}

void do_validate_inode_data(fs_t* pfs, struct dinode* pi, ushort inum)
{
	int nd;
	uchar addr;
	void* pb;
	int nde;
	struct dirent* pde;
	short hasreq = 0;

	if (inum == ROOTINO && pi->type != T_DIR)
	{
		// inode 1 must be allocated for root directory
		raise_exception(RootDirectoryError);
	}

	if (pi->nlink == 0)
	{
		raise_exception(InodeNotInUse);
	}
	else if (pi->type == T_DIR && pi->nlink != 1)
	{
		raise_exception(DirectoryLinkError);
	}

	// walk indode data
	for (nd = 0; nd * BSIZE < pi->size; nd++)
	{
		addr = get_and_validate_data_block_addr(pfs, pi, nd);

		if (pi->type == T_FILE)
		{
			//nothing to validate
			continue;
		}

		pb = bread(pfs, addr);
		// walk directory dirent
		for (nde = 0; 
			nd * BSIZE + nde < pi->size
				&& nde < BSIZE / sizeof(struct dirent); 
			nde++)
		{
			pde = (struct dirent*)pb + nde;

			if (strcmp(pde->name, ".") == 0)
			{
				if (pde->inum != inum)
				{
					raise_exception(DirectoryFormatError);
				}
				hasreq |= D_DOT;
			}
			else if (strcmp(pde->name, "..") == 0)
			{
				if (inum == ROOTINO)
				{
					if (pde->inum != ROOTINO)
					{
						raise_exception(RootDirectoryError);
					}
				}
				update_idata(pfs, pde->inum, inum);
				hasreq |= D_DOTDOT;
			}
			else 
			{
				update_idata(pfs, pde->inum, inum);
			}
		}
	}

	if (pi->type == T_DIR && hasreq != (D_DOT | D_DOTDOT))
	{
		raise_exception(DirectoryFormatError);
	}
}

uint get_and_validate_data_block_addr(fs_t* pfs, struct dinode* pi, int nd)
{
	uint addr;
	void* pb;

	if (nd < NDIRECT)
	{
		addr = pi->addrs[nd];
		do_validate_direct_addr(pfs, addr);
		return addr;
	}

	addr = pi->addrs[NDIRECT];
	// only validate the indirect block the first time it is referenced
	if (nd == NDIRECT)
	{
		do_validate_indirect_addr(pfs, addr);
	}

	// get addr from indirect data block
	nd -= NDIRECT;
	pb = bread(pfs, addr);
	addr = *((uint*)pb + nd);
	do_validate_direct_addr(pfs, addr);
	return addr;
}

void do_validate_direct_addr(fs_t* pfs, uint addr)
{
	if (!data_addr_is_valid(pfs, addr))
	{
		raise_exception(InvalidDirectAddress);
	}

	if (!data_block_reference_is_unique(pfs, addr))
	{
		raise_exception(DuplicateDirectAddressReference);
	}

	do_validate_data_block_in_use(pfs, addr);

	SETBIT(pfs->bblock, addr);
}

int data_addr_is_valid(fs_t* pfs, uint addr)
{
	return nmeta <= addr && addr < sb.size;
}

int data_block_reference_is_unique(fs_t* pfs, uint addr)
{
	return GETBIT(pfs->bblock, addr) == 0;
}

void do_validate_data_block_in_use(fs_t* pfs, uint addr)
{
	uchar* pb;
	int bi;

	pb = (uchar*)bread(pfs, BBLOCK(addr, sb));
	bi = addr % BPB;
	if (GETBIT(pb, bi) == 0)
	{
		raise_exception(FreeBlockInUse);
	}
}

void do_validate_indirect_addr(fs_t* pfs, uint addr)
{
	if (!data_addr_is_valid(pfs, addr))
	{
		raise_exception(InvalidIndirectAddress);
	}

	if (!data_block_reference_is_unique(pfs, addr))
	{
		raise_exception(DuplicateIndirectAddressReference);
	}

	do_validate_data_block_in_use(pfs, addr);

	SETBIT(pfs->bblock, addr);
}

void update_idata(fs_t* pfs, int childinum, int parentinum)
{
	if (childinum < ROOTINO || childinum >= pfs->szidata)
	{
		return;
	}

	pfs->idata[childinum].parent = parentinum;
	pfs->idata[childinum].ncount++;
}
