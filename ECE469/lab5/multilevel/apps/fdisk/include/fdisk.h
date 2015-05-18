#ifndef __FDISK_H__
#define __FDISK_H__

typedef unsigned int uint32;

#include "dfs_shared.h" // This gets us structures and #define's from main filesystem driver

// Where the boot record and superblock reside in the filesystem
#define FDISK_BOOT_FILESYSTEM_BLOCKNUM 0
// Starts after super block (which is in file system block 0, physical block 1)
#define FDISK_INODE_BLOCK_START 1
// Number of file system blocks to use for inodes
#define FDISK_INODE_NUM_BLOCKS (sizeof(dfs_inode) * FDISK_NUM_INODES / FDISK_FS_BLOCKSIZE) // = 36
#define FDISK_FBV_BLOCK_START (FDISK_INODE_BLOCK_START + FDISK_INODE_NUM_BLOCKS) // STUDENT*

#ifndef NULL
#define NULL (void *)0x0
#endif

//STUDENT: define additional parameters here, if any
#define FDISK_FS_BLOCKSIZE DFS_BLOCKSIZE // from dfs_shared.h
#define FDISK_NUM_INODES  192

// This gets disk_block struct to use with disk_write function
#define DISK_BLOCKSIZE 256
typedef struct disk_block {
  char data[DISK_BLOCKSIZE];
} disk_block;
#define DISK_SUCCESS 1
#define DISK_FAIL -1

#endif
