#include "usertraps.h"
#include "misc.h"

#include "fdisk.h"

dfs_superblock sb;
// dfs_inode inodes[FDISK_NUM_INODES];
uint32 fbv[DFS_FBV_MAX_NUM_WORDS];

int diskblocksize = 0; // These are global in order to speed things up
int disksize = 0;      // (i.e. fewer traps to OS to get the same number)

int FdiskWriteBlock(uint32 blocknum, dfs_block *b); //You can use your own function. This function
//calls disk_write_block() to write physical blocks to disk

static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

inline void SetFreeVector(int block, int state) {
  uint32 wd = block / 32;
  uint32 bitnum = block % 32;
  fbv[wd] = (fbv[wd] & invert(1 << bitnum)) | (state << bitnum);
}

void main (int argc, char *argv[])
{
  // STUDENT: put your code here. Follow the guidelines below. They are just the main steps.
  // You need to think of the finer details. You can use bzero() to zero out bytes in memory

  //Initializations and argc check
  int i;
  dfs_block dfsb;

  disksize = disk_size();
  diskblocksize = disk_blocksize();

  Printf("Sizeof inode: %d\n", sizeof(dfs_inode));

  // Need to invalidate filesystem before writing to it to make sure that the OS
  // doesn't wipe out what we do here with the old version in memory
  // You can use dfs_invalidate(); but it will be implemented in Problem 2. You can just do
  dfs_invalidate();
  sb.valid = 0;
  sb.blocksize = FDISK_FS_BLOCKSIZE;
  sb.total_blocks = disksize / sb.blocksize;
  sb.inode_start_block = FDISK_INODE_BLOCK_START;
  sb.total_inodes = FDISK_NUM_INODES;
  sb.fbv_start_block = FDISK_FBV_BLOCK_START;
  sb.fbv_num_words = (sb.total_blocks + 31) / 32;
  sb.fbv_num_blocks = ((sb.fbv_num_words * sizeof(uint32)) + (sb.blocksize - 1)) / sb.blocksize;
  sb.data_start_block = sb.fbv_start_block + sb.fbv_num_blocks;

  // Make sure the disk exists before doing anything else
  if(disk_create() == DISK_FAIL) {
    Printf("Failed to create disk\n");
    Exit();
  }
  Printf("Disk blocksize: %d FS blocksize: %d\n", diskblocksize, sb.blocksize);
  // Write all inodes as not in use and empty (all zeros)
  Printf("\nWriting the inodes\n");
  bzero(dfsb.data, sb.blocksize);
  for(i = sb.inode_start_block; i < sb.fbv_start_block; i++) {
    FdiskWriteBlock(i, &dfsb);
  }
  // Next, setup free block vector (fbv) and write free block vector to the disk
  for(i = 0; i < DFS_FBV_MAX_NUM_WORDS; i++) {
    fbv[i] = 0;
  }
  for(i = sb.data_start_block; i < sb.total_blocks; i++) {
    SetFreeVector(i, 1);
  }

  Printf("\nWriting the fvb\n");
  for(i = sb.fbv_start_block; i < sb.data_start_block; i++) {
    bcopy(&(((char*)fbv)[(i - sb.fbv_start_block) * sb.blocksize]), dfsb.data, sb.blocksize);
    FdiskWriteBlock(i, &dfsb);
  }

  Printf("\nWriting the MBR and super block\n");
  // Finally, setup superblock as valid filesystem and write superblock and boot record to disk:
  sb.valid = 1;
  // boot record is all zeros in the first physical block, and superblock structure goes into the second physical block
  bzero(dfsb.data, sb.blocksize);
  // this assumes dfs_blocksize > diskblocksize
  bcopy((char*)&sb, &(dfsb.data[diskblocksize]), sizeof(sb));
  FdiskWriteBlock(0, &dfsb);

  Printf("fdisk (%d): Formatted DFS disk for %d bytes.\n", getpid(), disksize);
}

int FdiskWriteBlock(uint32 blocknum, dfs_block *dfsb) {
  int m = sb.blocksize / diskblocksize;
  disk_block diskb;
  int i;
  for(i = 0; i < m; i++) {
    bcopy(&(dfsb->data[i * diskblocksize]), diskb.data, diskblocksize);
    Printf("Writing to physical block: %d\n", blocknum * m + i);
    if(disk_write_block(blocknum * m + i, &diskb) == DISK_FAIL) {
      Printf("Failed to write all to disk\n");
      return DISK_FAIL;
    }
  }
  return DISK_SUCCESS;
}
