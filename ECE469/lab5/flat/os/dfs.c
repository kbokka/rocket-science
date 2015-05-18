#include "ostraps.h"
#include "dlxos.h"
#include "traps.h"
#include "queue.h"
#include "disk.h"
#include "dfs.h"
#include "synch.h"

static dfs_inode inodes[DFS_INODES_MAX_NUM]; // all inodes
static dfs_superblock sb; // superblock
static uint32 fbv[DFS_FBV_MAX_NUM_WORDS]; // Free block vector

static lock_t dfs_fbv_lock;
static lock_t dfs_inode_lock;

static unsigned int negativeone = 0xFFFFFFFF;
static inline unsigned int invert(uint32 n) { return n ^ negativeone; }

// You have already been told about the most likely places where you should use locks. You may use
// additional locks if it is really necessary.

// STUDENT: put your file system level functions below.
// Some skeletons are provided. You can implement additional functions.
inline void SetFreeVector(int block, int state) {
  uint32 wd = block / 32;
  uint32 bitnum = block % 32;
  fbv[wd] = (fbv[wd] & invert(1 << bitnum)) | (state << bitnum);
  printf("[DEBUG] Set block: %d state:%d\n", block, state);
  dbprintf('d', "Set fbv entry %d to 0x%x\n", wd, fbv[wd]);
}

///////////////////////////////////////////////////////////////////
// Non-inode functions first
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------
// DfsModuleInit is called at boot time to initialize things and
// open the file system for use.
//-----------------------------------------------------------------

void DfsModuleInit() {
// You essentially set the file system as invalid and then open
// using DfsOpenFileSystem().
  sb.valid = 0;
  dfs_fbv_lock = LockCreate();
  dfs_inode_lock = LockCreate();
  DfsOpenFileSystem();
}

//-----------------------------------------------------------------
// DfsInavlidate marks the current version of the filesystem in
// memory as invalid.  This is really only useful when formatting
// the disk, to prevent the current memory version from overwriting
// what you already have on the disk when the OS exits.
//-----------------------------------------------------------------

void DfsInvalidate() {
// This is just a one-line function which sets the valid bit of the
// superblock to 0.
  sb.valid = 0;
}

//-------------------------------------------------------------------
// DfsOpenFileSystem loads the file system metadata from the disk
// into memory.  Returns DFS_SUCCESS on success, and DFS_FAIL on
// failure.
//-------------------------------------------------------------------

int DfsOpenFileSystem() {
  disk_block diskb;
  dfs_block dfsb;
  char* inbytes;
  unsigned int i;
//Basic steps:
// Check that filesystem is not already open
  if(sb.valid) {
    printf("DfsOpenFilesystem: File system is already open\n");
    return DFS_FAIL;
  }
// Read superblock from disk.  Note this is using the disk read rather
// than the DFS read function because the DFS read requires a valid
// filesystem in memory already, and the filesystem cannot be valid
// until we read the superblock. Also, we don't know the block size
// until we read the superblock, either.
  DiskReadBlock(1, &diskb);

// Copy the data from the block we just read into the superblock in memory
  bcopy(diskb.data, (char*) &sb, sizeof(sb));
  // sb should be valid at this point since sb was supposed to be valid on disk
  if(!sb.valid) {
    printf("DfsOpenFileSystem: Filesystem on disk is not valid\n");
    return DFS_FAIL;
  }

// All other blocks are sized by virtual block size:
// Read inodes
  inbytes = (char*) inodes;
  for(i = sb.inode_start_block; i < sb.fbv_start_block; i++) {
    DfsReadBlock(i, &dfsb);
    bcopy(dfsb.data, &(inbytes[(i - sb.inode_start_block) * sb.blocksize]), sb.blocksize);
  }
// Read free block vector
  inbytes = (char*) fbv;
  for(i = sb.fbv_start_block; i < sb.data_start_block; i++) {
    DfsReadBlock(i, &dfsb);
    bcopy(dfsb.data, &(inbytes[(i - sb.fbv_start_block) * sb.blocksize]), sb.blocksize);
  }
// Change superblock to be invalid, write back to disk, then change
// it back to be valid in memory
  sb.valid = 0;
  bzero(diskb.data, DISK_BLOCKSIZE);
  bcopy((char*)&sb, diskb.data, sizeof(sb));
  DiskWriteBlock(1, &diskb);
  sb.valid = 1;
  return DFS_SUCCESS;
}

//-------------------------------------------------------------------
// DfsCloseFileSystem writes the current memory version of the
// filesystem metadata to the disk, and invalidates the memory's
// version.
//-------------------------------------------------------------------
int DfsCloseFileSystem() {
  dfs_block dfsb;
  disk_block diskb;
  int i;
  char* inbytes;
  if(!sb.valid) {
    printf("DfsCloseFileSystem: Filesystem not already open\n");
    return DFS_FAIL;
  }

// Write inodes
  inbytes = (char*) inodes;
  for(i = sb.inode_start_block; i < sb.fbv_start_block; i++) {
    bcopy(&(inbytes[(i - sb.inode_start_block)  * sb.blocksize]), dfsb.data, sb.blocksize);
    DfsWriteBlock(i, &dfsb);
  }
// Write free block vector
  inbytes = (char*) fbv;
  for(i = sb.fbv_start_block; i < sb.data_start_block; i++) {
    bcopy(&(inbytes[(i - sb.fbv_start_block) * sb.blocksize]), dfsb.data, sb.blocksize);
    DfsWriteBlock(i, &dfsb);
  }

  bzero(diskb.data, DISK_BLOCKSIZE);
  bcopy((char*)(&sb), diskb.data, sizeof(sb));
  if(DiskWriteBlock(1, &diskb) == DISK_FAIL) {
    printf("DfsCloseFileSystem: DiskWriteBlock failed\n");
    return DFS_FAIL;
  }
  sb.valid = 0;
  return DFS_SUCCESS;
}


//-----------------------------------------------------------------
// DfsAllocateBlock allocates a DFS block for use. Remember to use
// locks where necessary.
//-----------------------------------------------------------------

uint32 DfsAllocateBlock() {
  int i;
  uint32 bitnum;
  uint32 vector;
// Check that file system has been validly loaded into memory
  if(!sb.valid) {
    printf("DfsAllocateBlock: Filesystem is not validly loaded into memory\n");
    return DFS_FAIL;
  }
  while(LockHandleAcquire(dfs_fbv_lock) != SYNC_SUCCESS);
// Find the first free block using the free block vector (FBV), mark it in use
  for(i = 0; i < sb.fbv_num_words; i++) {
    if(fbv[i] != 0) {
      break;
    }
  }
  if(i == sb.fbv_num_words) {
    printf("DfsAllocateBlock: Could not allocate block\n");
    return DFS_FAIL;
  }
  vector = fbv[i];
  for(bitnum = 0; (vector & (1 << bitnum)) == 0; bitnum++){}
  fbv[i]  &= invert(1 << bitnum);
  vector = (i * 32) + bitnum;
  dbprintf('d', "DfsAllocateBlock: Allocated block %d\n", vector);
  LockHandleRelease(dfs_fbv_lock);
// Return handle to block
  return vector;
}


//-----------------------------------------------------------------
// DfsFreeBlock deallocates a DFS block.
//-----------------------------------------------------------------
int DfsFreeBlock(uint32 blocknum) {
  if(!sb.valid) {
    printf("DfsFreeBlock: Filesystem is not validly loaded into memory\n");
    return DFS_FAIL;
  }
  while(LockHandleAcquire(dfs_fbv_lock) != SYNC_SUCCESS);
  SetFreeVector(blocknum, 1);
  LockHandleRelease(dfs_fbv_lock);
  dbprintf('d', "DfsFreeBlock: Freed block %d\n", blocknum);
  return DFS_SUCCESS;
}


//-----------------------------------------------------------------
// DfsReadBlock reads an allocated DFS block from the disk
// (which could span multiple physical disk blocks).  The block
// must be allocated in order to read from it.  Returns DFS_FAIL
// on failure, and the number of bytes read on success.
//-----------------------------------------------------------------
int DfsReadBlock(uint32 blocknum, dfs_block *b) {
  disk_block diskb;
  int diskblocksize;
  int factor;
  int i;
  int count = 0;
  if(!sb.valid) {
    printf("DfsReadBlock: file system is not valid\n");
    return DFS_FAIL;
  }
  // if(!(fbv[blocknum / 32] & (1 << (blocknum % 32)))) {
  //   printf("DfsReadBlock: the block %d is not allocated\n", blocknum);
  //   return DFS_FAIL;
  // }

  diskblocksize = DiskBytesPerBlock();
  factor = sb.blocksize / diskblocksize;
  for(i = 0; i < factor; i++) {
    if(DiskReadBlock((blocknum * factor) + i, &diskb) == DISK_FAIL) {
      printf("DfsReadBlock: Disk read failed copied %d of %d\n", count, sb.blocksize);
      break;
    }
    bcopy(diskb.data, &(b->data[i * diskblocksize]), diskblocksize);
    count += diskblocksize;
  }
  return count;
}

//-----------------------------------------------------------------
// DfsWriteBlock writes to an allocated DFS block on the disk
// (which could span multiple physical disk blocks).  The block
// must be allocated in order to write to it.  Returns DFS_FAIL
// on failure, and the number of bytes written on success.
//-----------------------------------------------------------------
int DfsWriteBlock(uint32 blocknum, dfs_block *b){
  disk_block diskb;
  int diskblocksize;
  int factor;
  int i;
  int count = 0;
  if(fbv[blocknum / 32] & (1 << (blocknum % 32))) {
    printf("DfsWriteBlock: the block %d is not allocated\n", blocknum);
    return DFS_FAIL;
  }
  diskblocksize = DiskBytesPerBlock();
  factor = sb.blocksize / diskblocksize;
  for(i = 0; i < factor; i++) {
    bcopy(&(b->data[i * diskblocksize]), diskb.data, diskblocksize);
    if(DiskWriteBlock((blocknum * factor) + i, &diskb) == DISK_FAIL) {
      printf("DfsWriteBlock: Disk write failed copied %d of %d\n", count, sb.blocksize);
      break;
    }
    count += diskblocksize;
  }
  return count;
}


////////////////////////////////////////////////////////////////////////////////
// Inode-based functions
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------
// DfsInodeFilenameExists looks through all the inuse inodes for
// the given filename. If the filename is found, return the handle
// of the inode. If it is not found, return DFS_FAIL.
//-----------------------------------------------------------------
uint32 DfsInodeFilenameExists(char *filename) {
  uint32 i;
  if(!sb.valid) {
    printf("DfsInodeFilenameExists: invalid file system\n");
    return DFS_FAIL;
  }
  for(i = 0; i < sb.total_inodes; i++) {
    if(!inodes[i].valid) { continue; }
    if(dstrncmp(filename, inodes[i].filename, DFS_FILENAME_LENGTH) == 0) {
      return i;
    }
  }
  return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeOpen: search the list of all inuse inodes for the
// specified filename. If the filename exists, return the handle
// of the inode. If it does not, allocate a new inode for this
// filename and return its handle. Return DFS_FAIL on failure.
// Remember to use locks whenever you allocate a new inode.
//-----------------------------------------------------------------
uint32 DfsInodeOpen(char *filename) {
  uint32 i;
  if(!sb.valid) {
    printf("DfsInodeOpen: invalid file system\n");
    return DFS_FAIL;
  }
  i = DfsInodeFilenameExists(filename);
  if(i != DFS_FAIL) {
    printf("[DEBUG] Opened inode:%d\n", i);
    return i;
  }
  while(LockHandleAcquire(dfs_inode_lock) != SYNC_SUCCESS);
  for(i = 0; i < sb.total_inodes; i++) {
    if(inodes[i].valid) { continue; }
    inodes[i].valid = 1;
    inodes[i].filesize = 0;
    dstrncpy(inodes[i].filename, filename, DFS_FILENAME_LENGTH);
    break;
  }
  LockHandleRelease(dfs_inode_lock);
  printf("[DEBUG] Opened inode:%d\n", i);
  return i;
}


//-----------------------------------------------------------------
// DfsInodeDelete de-allocates any data blocks used by this inode,
// including the indirect addressing block if necessary, then mark
// the inode as no longer in use. Use locks when modifying the
// "inuse" flag in an inode.Return DFS_FAIL on failure, and
// DFS_SUCCESS on success.
//-----------------------------------------------------------------
int DfsInodeDelete(uint32 handle) {
  int i;
  dfs_block dfsb;
  int* inints = (int*) dfsb.data;
  if(!sb.valid) {
    printf("DfsInodeDelete: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeDelete: invalid inode\n");
    return DFS_FAIL;
  }
  while(LockHandleAcquire(dfs_fbv_lock) != SYNC_SUCCESS);
  if(DFS_IS_BLOCK_ENTRY_VALID(inodes[handle].indir_block_entry)) {
    DfsReadBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    for(i = 0; i < (sb.blocksize / sizeof(int)); i++) {
      if(DFS_IS_BLOCK_ENTRY_VALID(inints[i])) {
        SetFreeVector(DFS_GET_BLOCK_NUM(inints[i]), 1);
      }
    }
    // clear out the indirect block entry table
    bzero(dfsb.data, sb.blocksize);
    DfsWriteBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    SetFreeVector(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), 1);
    inodes[handle].indir_block_entry = DFS_SET_BLOCK_ENTRY(0, 0);
  }
  for(i = 0; i < 10; i++) {
    if(DFS_IS_BLOCK_ENTRY_VALID(inodes[handle].direct_block_entries[i])) {
      SetFreeVector(DFS_GET_BLOCK_NUM(inodes[handle].direct_block_entries[i]), 1);
      inodes[handle].direct_block_entries[i] = DFS_SET_BLOCK_ENTRY(0, 0);
    }
  }
  LockHandleRelease(dfs_fbv_lock);

  while(LockHandleAcquire(dfs_inode_lock) != SYNC_SUCCESS);
  inodes[handle].valid = 0;
  LockHandleRelease(dfs_inode_lock);
  return DFS_SUCCESS;
}

//-----------------------------------------------------------------
// DfsInodeFilesize simply returns the size of an inode's file.
// This is defined as the maximum virtual byte number that has
// been written to the inode thus far. Return DFS_FAIL on failure.
//-----------------------------------------------------------------

uint32 DfsInodeFilesize(uint32 handle) {
  if(!sb.valid) {
    printf("DfsInodeFilesize: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeFilesize: invalid inode\n");
    return DFS_FAIL;
  }
  return inodes[handle].filesize;
}

int DfsInodeTruncateFile(uint32 handle) {
  int i;
  dfs_block dfsb;
  int* inints = (int*) dfsb.data;
  
  if(!sb.valid) {
    printf("DfsInodeTruncateFile: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeTruncateFile: invalid inode\n");
    return DFS_FAIL;
  }

  while(LockHandleAcquire(dfs_fbv_lock) != SYNC_SUCCESS);
  if(DFS_IS_BLOCK_ENTRY_VALID(inodes[handle].indir_block_entry)) {
    DfsReadBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    for(i = 0; i < (sb.blocksize / sizeof(int)); i++) {
      if(DFS_IS_BLOCK_ENTRY_VALID(inints[i])) {
        SetFreeVector(DFS_GET_BLOCK_NUM(inints[i]), 1);
      }
    }
    // clear out the indirect block entry table
    bzero(dfsb.data, sb.blocksize);
    DfsWriteBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    SetFreeVector(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), 1);
    inodes[handle].indir_block_entry = DFS_SET_BLOCK_ENTRY(0, 0);
  }
  for(i = 0; i < 10; i++) {
    if(DFS_IS_BLOCK_ENTRY_VALID(inodes[handle].direct_block_entries[i])) {
      SetFreeVector(DFS_GET_BLOCK_NUM(inodes[handle].direct_block_entries[i]), 1);
      inodes[handle].direct_block_entries[i] = DFS_SET_BLOCK_ENTRY(0, 0);
    }
  }
  LockHandleRelease(dfs_fbv_lock);

  inodes[handle].filesize = 0;
  return DFS_SUCCESS;
}

//-----------------------------------------------------------------
// DfsInodeAllocateVirtualBlock allocates a new filesystem block
// for the given inode, storing its blocknumber at index
// virtual_blocknumber in the translation table. If the
// virtual_blocknumber resides in the indirect address space, and
// there is not an allocated indirect addressing table, allocate it.
// Return DFS_FAIL on failure, and the newly allocated file system
// block number on success.
//-----------------------------------------------------------------

uint32 DfsInodeAllocateVirtualBlock(uint32 handle, uint32 virtual_blocknum) {
  dfs_block dfsb;
  int blocknum;
  int* block_entry_table;
  if(!sb.valid) {
    printf("DfsInodeAllocateVirtualBlock: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeAllocateVirtualBlock: invalid inode\n");
    return DFS_FAIL;
  }

  if(virtual_blocknum >= 10) {
    block_entry_table = (int*) dfsb.data;
    if(!DFS_IS_BLOCK_ENTRY_VALID(inodes[handle].indir_block_entry)) {
      blocknum = DfsAllocateBlock();
      if(blocknum == DFS_FAIL) {
        printf("DfsInodeAllocateVirtualBlock: DfsAllocateBlock failed\n");
        return DFS_FAIL;
      }
      bzero(dfsb.data, sb.blocksize);
      DfsWriteBlock(blocknum, &dfsb);
      inodes[handle].indir_block_entry = DFS_SET_BLOCK_ENTRY(blocknum, 1);
    }
    DfsReadBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    virtual_blocknum -= 10;
    // Before allocating new block
    // Extra check to make sure that there is no already valid entry here
    if(DFS_IS_BLOCK_ENTRY_VALID(block_entry_table[virtual_blocknum])) {
      return DFS_GET_BLOCK_NUM(block_entry_table[virtual_blocknum]);
    }
    else {
      blocknum = DfsAllocateBlock();
      if(blocknum == DFS_FAIL) {
        printf("DfsInodeAllocateVirtualBlock: DfsAllocateBlock failed\n");
        return DFS_FAIL;
      }
      block_entry_table[virtual_blocknum] = DFS_SET_BLOCK_ENTRY(blocknum, 1);
      DfsWriteBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    }
    return blocknum;
  }
  else {
    block_entry_table = inodes[handle].direct_block_entries;
    // Before allocating new block
    // Extra check to make sure that there is no already valid entry here
    if(DFS_IS_BLOCK_ENTRY_VALID(block_entry_table[virtual_blocknum])) {
      return DFS_GET_BLOCK_NUM(block_entry_table[virtual_blocknum]);
    }
    else {
      blocknum = DfsAllocateBlock();
      if(blocknum == DFS_FAIL) {
        printf("DfsInodeAllocateVirtualBlock: DfsAllocateBlock failed\n");
        return DFS_FAIL;
      }
      block_entry_table[virtual_blocknum] = DFS_SET_BLOCK_ENTRY(blocknum, 1);
      return blocknum;
    }
  }
  printf("---THIS SHOULD NOT HAPPEN---\n");
  return DFS_FAIL;
}

//-----------------------------------------------------------------
// DfsInodeTranslateVirtualToFilesys translates the
// virtual_blocknum to the corresponding file system block using
// the inode identified by handle. Return DFS_FAIL on failure.
//-----------------------------------------------------------------
uint32 DfsInodeTranslateVirtualToFilesys(uint32 handle, uint32 virtual_blocknum) {
  dfs_block dfsb;
  int* block_entry_table;
  if(!sb.valid) {
    printf("DfsInodeTranslateVirtualToFilesys: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeTranslateVirtualToFilesys: invalid inode\n");
    return DFS_FAIL;
  }

  if(virtual_blocknum >= 10) {
    block_entry_table = (int*) dfsb.data;
    if(!DFS_IS_BLOCK_ENTRY_VALID(inodes[handle].indir_block_entry)) {
      printf("DfsInodeTranslateVirtualToFilesys: invalid block entry in indirect blocks %d\n", virtual_blocknum);
      return DFS_FAIL;
    }
    DfsReadBlock(DFS_GET_BLOCK_NUM(inodes[handle].indir_block_entry), &dfsb);
    virtual_blocknum -= 10;
    if(DFS_IS_BLOCK_ENTRY_VALID(block_entry_table[virtual_blocknum])) {
      return DFS_GET_BLOCK_NUM(block_entry_table[virtual_blocknum]);
    }
    else {
      printf("DfsInodeTranslateVirtualToFilesys: invalid block's block entry in indirect blocks %d\n", virtual_blocknum);
      return DFS_FAIL;
    }
  }
  else {
    block_entry_table = inodes[handle].direct_block_entries;
    if(DFS_IS_BLOCK_ENTRY_VALID(block_entry_table[virtual_blocknum])) {
      return DFS_GET_BLOCK_NUM(block_entry_table[virtual_blocknum]);
    }
    else {
      printf("DfsInodeTranslateVirtualToFilesys: invalid block entry in direct blocks %d\n", virtual_blocknum);
      return DFS_FAIL;
    }
  }
  printf("---THIS SHOULD NOT HAPPEN---\n");
  return DFS_FAIL;
}

//-----------------------------------------------------------------
// DfsInodeReadBytes reads num_bytes from the file represented by
// the inode handle, starting at virtual byte start_byte, copying
// the data to the address pointed to by mem. Return DFS_FAIL on
// failure, and the number of bytes read on success.
//-----------------------------------------------------------------
int DfsInodeReadBytes(uint32 handle, void *mem, int start_byte, int num_bytes) {
  int cur_byte = start_byte;
  int blocknum;
  int data_bytes;
  int byte_count = 0;
  dfs_block dfsb;
  char* mem_bytes = (char*) mem;

  if(!sb.valid) {
    printf("DfsInodeReadBytes: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeReadBytes: invalid inode\n");
    return DFS_FAIL;
  }
  while(byte_count < num_bytes) {
    blocknum = DfsInodeTranslateVirtualToFilesys(handle, cur_byte / sb.blocksize);
    if(blocknum == DFS_FAIL) {
      printf("Error: Trying to read from part of file that wasn't written already\n");
      return DFS_FAIL;
    }
    if(DfsReadBlock(blocknum, &dfsb) == DFS_FAIL) {
      printf("DfsInodeReadBytes: DfsReadBlock failed\n");
      break;
    }
    data_bytes = sb.blocksize - (cur_byte % sb.blocksize);
    if((byte_count + data_bytes) > num_bytes) {
      // remove the extra data bytes
      data_bytes = data_bytes - ((byte_count + data_bytes) - num_bytes);
    }
    bcopy(&(dfsb.data[cur_byte % sb.blocksize]), &(mem_bytes[byte_count]), data_bytes);
    byte_count += data_bytes;
    cur_byte = start_byte + byte_count;
  }
  return byte_count;
}


//-----------------------------------------------------------------
// DfsInodeWriteBytes writes num_bytes from the memory pointed to
// by mem to the file represented by the inode handle, starting at
// virtual byte start_byte. Note that if you are only writing part
// of a given file system block, you'll need to read that block
// from the disk first. Return DFS_FAIL on failure and the number
// of bytes written on success.
//-----------------------------------------------------------------
int DfsInodeWriteBytes(uint32 handle, void *mem, int start_byte, int num_bytes) {
  int cur_byte = start_byte;
  int blocknum;
  int data_bytes;
  int byte_count = 0;
  dfs_block dfsb;
  char* mem_bytes = (char*) mem;

  if(!sb.valid) {
    printf("DfsInodeWriteBytes: invalid file system\n");
    return DFS_FAIL;
  }
  if(!inodes[handle].valid) {
    printf("DfsInodeWriteBytes: invalid inode\n");
    return DFS_FAIL;
  }
  while(byte_count < num_bytes) {
    // allocate block first checks whether there's existing otherwise allocates
    blocknum = DfsInodeAllocateVirtualBlock(handle, cur_byte / sb.blocksize);
    data_bytes = sb.blocksize - (cur_byte % sb.blocksize);
    if(data_bytes == sb.blocksize) {
      // the start is blocksize aligned
      // check if end is not block aligned
      if((byte_count + data_bytes) > num_bytes) {
        // remove the extra data bytes
        data_bytes = data_bytes - ((byte_count + data_bytes) - num_bytes);
        // not block aligned so read whole block
        if(DfsReadBlock(blocknum, &dfsb) == DFS_FAIL) {
          printf("DfsInodeReadBytes: DfsReadBlock failed\n");
          break;
        }
      }
    }
    else {
      // start is not block algined
      // check if end is not block aligned
      if((byte_count + data_bytes) > num_bytes) {
        // remove the extra data bytes
        data_bytes = data_bytes - ((byte_count + data_bytes) - num_bytes);
      }
      // We need to do this any way since start is not block aligned
      if(DfsReadBlock(blocknum, &dfsb) == DFS_FAIL) {
        printf("DfsInodeWriteBytes: DfsReadBlock failed\n");
        break;
      }
    }
    bcopy(&(mem_bytes[byte_count]), &(dfsb.data[cur_byte % sb.blocksize]), data_bytes);
    if(DfsWriteBlock(blocknum, &dfsb) == DFS_FAIL) {
      printf("DfsInodeWriteBytes: DfsWriteBlock failed\n");
      break;
    }

    byte_count += data_bytes;
    cur_byte = start_byte + byte_count;
  }
  
  if(inodes[handle].filesize < (start_byte + byte_count)) {
    inodes[handle].filesize =  start_byte + byte_count;
  }

  return byte_count;
}
