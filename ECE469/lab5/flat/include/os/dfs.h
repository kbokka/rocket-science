#ifndef __DFS_H__
#define __DFS_H__

#include "dfs_shared.h"

#define DFS_GET_BLOCK_NUM(block_entry) ((block_entry) >> 1)
#define DFS_IS_BLOCK_ENTRY_VALID(block_entry) ((block_entry) & 0x1)
#define DFS_SET_BLOCK_ENTRY(blocknum, valid) (((blocknum) << 1) | (valid))

void DfsModuleInit();
void DfsInvalidate();
int DfsOpenFileSystem();
int DfsCloseFileSystem();
uint32 DfsAllocateBlock();
int DfsFreeBlock(uint32 blocknum);
int DfsReadBlock(uint32 blocknum, dfs_block *b);
int DfsWriteBlock(uint32 blocknum, dfs_block *b);
uint32 DfsInodeFilenameExists(char *filename);
uint32 DfsInodeOpen(char *filename);
int DfsInodeDelete(uint32 handle);
uint32 DfsInodeFilesize(uint32 handle);
int DfsInodeTruncateFile(uint32 handle); // this new function is used when writing
uint32 DfsInodeAllocateVirtualBlock(uint32 handle, uint32 virtual_blocknum);
uint32 DfsInodeTranslateVirtualToFilesys(uint32 handle, uint32 virtual_blocknum);
int DfsInodeReadBytes(uint32 handle, void *mem, int start_byte, int num_bytes);
int DfsInodeWriteBytes(uint32 handle, void *mem, int start_byte, int num_bytes);

#endif
