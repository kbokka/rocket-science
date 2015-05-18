#ifndef __DFS_SHARED__
#define __DFS_SHARED__

typedef unsigned int inode_t;

typedef struct dfs_superblock {
  // STUDENT*: put superblock internals here
  // - a valid indicator for the file system
  // - the file system block size
  // - the total number of file system blocks
  // - the starting file system block number for the array of inodes
  // - the number of inodes in the inodes array
  // - the starting file system block number for the free block vector.
  char valid;
  unsigned int blocksize;
  unsigned int total_blocks;
  unsigned int inode_start_block;
  unsigned int total_inodes;
  unsigned int fbv_start_block;
  unsigned int fbv_num_words;
  unsigned int fbv_num_blocks;
  unsigned int data_start_block;
} dfs_superblock;

#define DFS_BLOCKSIZE 512  // Must be an integer multiple of the disk blocksize

typedef struct dfs_block {
  char data[DFS_BLOCKSIZE];
} dfs_block;

#define JUNK_LENGTH 44
typedef struct dfs_inode {
  // STUDENT*: put inode structure internals here
  // IMPORTANT: sizeof(dfs_inode) MUST return 96 in order to fit in enough
  // inodes in the filesystem (and to make your life easier).  To do this,
  // adjust the maximumm length of the filename until the size of the overall inode
  // is 96 bytes.
  // - an in use indicator to tell if an inode is free or in use
  // - the size of the file this inode represents (i.e. the maximum byte that has been
  //		written to this file)
  // - the filename, which is just a string
  // - a table of direct address translations for the first 10 virtual blocks
  // - a block number of a file system block on the disk which holds a table of
  //		indirect address translations for the virtual blocks beyond the first 10.
  unsigned char valid;
  unsigned char is_dir;
  unsigned char owner_id;
  unsigned char permissions;
  unsigned int filesize;
  unsigned int direct_block_entries[10];
  unsigned int indir_block_entry;
  char junk[JUNK_LENGTH];
} dfs_inode;

#define DFS_MAX_FILESYSTEM_SIZE 0x1000000  // 16MB

#define DFS_MIN_BLOCKSIZE 64

// DFS_MAX_FILESYSTEM_SIZE / 64 = total_blocks if blocksize is 64
// (total_blocks + 31) / 32 = FBV_NUM_WORDS
#define DFS_FBV_MAX_NUM_WORDS (((DFS_MAX_FILESYSTEM_SIZE / DFS_MIN_BLOCKSIZE) + 31)  / 32)
#define DFS_INODES_MAX_NUM 200

#define DFS_FAIL -1
#define DFS_SUCCESS 1

#endif