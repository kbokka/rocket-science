#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "dfs.h"
#include "files.h"
#include "synch.h"

// You have already been told about the most likely places where you should use locks. You may use 
// additional locks if it is really necessary.
static file_descriptor fds[FILE_MAX_OPEN_FILES];
static lock_t fds_lock;
extern PCB* currentPCB;

//---------------------------------------------------------------------------
//	checkPermission
//
//	Function to check whether the passed bits of a file are set. If the
//	specified permission is granted, it returns 1, otherwise it returns 0.
//---------------------------------------------------------------------------
static int checkPermission(inode_t id, char mode)
{
  INode *node;
  char flmode;

  node = getInodeAddress(id);

  if(node==NULL)
  {
    return 0;				//inode not valid
  }
  
  flmode = node->permission;

  if(node->ownerid == GetPidFromAddress(currentPCB))
  {
    mode<<=3;
  }
  
  flmode &= mode;			//Mask all the bits other than the bits being tested
  flmode ^= mode;			//XOR with bits being tested. Result is 0 only if all the bits match 

  if(flmode)
  {
    return 0;				//permission denied
  }
  else
  {
    return 1;				//permission granted
  }
}

//---------------------------------------------------------------------------
//	getOneName
//	
//	This function works much like strtok. The only difference is that the
//	field delimiter is fixed at '/', and you have to specify the
//	destination string where the token could be returned. The string dst
//	should have at least 31 bytes of space. This function also checks for
//	some error conditions such as extremely long filenames and successive
//	'/' characters. On success it returns the length of the string written
//	in dst (not to exceed 30). On error, it returns -1. If the string src
//	starts with a '/', the first '/' is ignored.
//
//	Example: Consider src = "/a/b/cd"
//		len = getOneName(dst, src); 
//			//returns dst = "a", len = 1;
//		len = getOneName(dst, NULL);
//			//returns dst = "b", len = 1;
//		len = getOneName(dst, NULL);
//			//returns dst = "cd", len = 2;
//		len = getOneName(dst, NULL);
//			//returns dst = "", len = 0;
//	
//	Note that the successive calls should be made with src = NULL. If src
//	is not NULL, the string passed is parsed from the beginnning.
//--------------------------------------------------------------------------
static char* str;
static int getOneName(char *dst, char *src)
{
  int count = 0;

  if(src!=NULL)
  {
    if(*src=='/')
      src++;
    if(*src=='/')
      return -1;			//successive '/' not allowed
    str = src;	
  }
  for(;*str!='\0';str++)
  {
    dst[count] = *str;
    count++;
    if(*str=='/')
    {
      str++;
      count--;
      if(*str =='/')
      {
        return -1;			//successive '/' not allowed
      }
      break;
    }
    else
    {
      if(count==71) 
      {
       return -1;			//Filename too long
      }
    }
  }
  dst[count] = '\0';
  return count;
}

//--------------------------------------------------------------------------
//	MakeInodeFromPath
//
//	Given a path, and inode description, create that inode and return the
//	inode identifier. If a file corresponding to the path already exists, 
//	it returns FILE_EXISTS. This function is used both for creating a file 
//	and creating a directory. If the path provided is not valid, then this
//	function returns INVALID_PATH. If any directory along the path does
//	not have EXECUTE permission, or the parent directory of the leaf does
//	not have write permission, this function returns PERMISSION_DENIED.
//--------------------------------------------------------------------------
static inode_t tmp_makeInodeFromPath_last_inode;
static char 	 tmp_makeInodeFromPath_last_token[FILE_MAX_FILENAME_LENGTH];
static inode_t makeInodeFromPath(char *path, int mode, int type)
{
  //This function is not mandatory, and you can skip writing it if you want.
  //But we strongly recommend writing this function as you can use this
  //function to create files as well as directories.
  char token[FILE_MAX_FILENAME_LENGTH];
  int token_length = 1;
	inode_t n_inode;
	token_length = getOneName(token, path);
  while(token_length) {
  	if(token_length == -1) {
  		return INVALID_PATH;
  	}
  	tmp_makeInodeFromPath_last_inode = n_inode;
  	dstrcpy(tmp_makeInodeFromPath_last_token, token);
  	n_inode = DfsInodeFilenameExists(token);
  	if(tmp_makeInodeFromPath_last_inode != DFS_FAIL) {
  			if(!DfsGetInodeAddress(n_inode)->is_dir) {
  				break;
  			}
  	}
  	else {
  		break;
  	}
		token_length = getOneName(token, NULL);
	}
	token_length = getOneName(token, NULL);
	if(token_length != 0) {
		return INVALID_PATH;
	}
	return FILE_EXISTS;
	// DfsInodeOpen();
}

//----------------------------------------------------------------------------
//	DeleteInodeGivenParent
//
//	Given the path, this function deletes the inode corresponding to that
//	path, and updates the directory entry in the parent directory.
//----------------------------------------------------------------------------
static int deleteInodeGivenParent(char *leafName, inode_t leaf, inode_t parent)
{
  //This function is not mandatory, and you can skip writing it if you want.
  //But we strongly recommend writing this function as you can use this
  //function to delete files as well as directories.
}

// STUDENT: put your file-level functions here
void FileModuleInit() {
	int i;
	for(i = 0; i < FILE_MAX_OPEN_FILES; i++) {
		fds[i].valid = 0;
	}
	fds_lock = LockCreate();
}
// open the given filename with one of three possible modes: "r", "w", or "rw".
// Return FILE_FAIL on failure (e.g., when a process tries to open a file that is already open for another process), and the handle of a file descriptor on success.
// Remember to use locks whenever you allocate a new file descriptor.
int FileOpen(char *filename, char *mode) {
	int i;
	int inode_handle = DfsInodeFilenameExists(filename);
	int fd_handle = -1;
	char flag;

	if(mode[0] == 'r') {
		if(mode[1] == 'w') {
			flag = FILE_FLAG_RW;
		}
		else {
			flag = FILE_FLAG_R;
		}
	}
	else {
		// mode[0] == 'w'
		flag = FILE_FLAG_W;
	}

	if(inode_handle == DFS_FAIL && FILE_CHECK_FLAG_R(flag)) {
		// for read mode file needs to exist for sure
		dbprintf('a', "FileOpen: File does not exist.\n");
		return FILE_FAIL;
	}
	if(inode_handle == DFS_FAIL) {
		printf("[DEBUG] File didn't exist\n");
	}

	while(LockHandleAcquire(fds_lock) != SYNC_SUCCESS);
	// search for open file desc
	for(i = 0; i < FILE_MAX_OPEN_FILES; i++) {
		if(fds[i].valid) {
			if(fds[i].inode_handle == inode_handle) {
				dbprintf('a', "FileOpen: Inode is taken\n");
				LockHandleRelease(fds_lock);
				return DFS_FAIL;
			}
		}
		else {
			fd_handle = i;
			// an available file descriptor
			break;
		}
	}
	// continue searching through to make sure the file is not already open
	for(; i < FILE_MAX_OPEN_FILES; i++) {
		if(fds[i].valid) {
			if(fds[i].inode_handle == inode_handle) {
				dbprintf('a', "FileOpen: Inode is taken\n");
				LockHandleRelease(fds_lock);
				return DFS_FAIL;
			}
		}
	}

	fds[fd_handle].valid = 1;
	dstrncpy(fds[fd_handle].filename, filename, FILE_MAX_FILENAME_LENGTH);
	if(inode_handle == DFS_FAIL) {
		fds[fd_handle].inode_handle = DfsInodeOpen(filename);
	}
	else {
		fds[fd_handle].inode_handle = inode_handle;
	}
	if(flag == FILE_FLAG_W || flag == FILE_FLAG_RW) {
		// if the file existed and mode is W or RW
		DfsInodeTruncateFile(fds[fd_handle].inode_handle);
	}
	// printf("[DEBUG] Opened file with inode: %d\n", fds[fd_handle].inode_handle);
	fds[fd_handle].cur_pos = 0;
	fds[fd_handle].flag = flag;

	LockHandleRelease(fds_lock);

	return fd_handle;
}

// close the given file descriptor handle. Return FILE_FAIL on failure, and FILE_SUCCESS on success.
int FileClose(int handle) {
	if(!fds[handle].valid) {
		return FILE_FAIL;
	}
	// just to make sure since an strncpy on FileOpen does not really replace all chars
	bzero(fds[handle].filename, FILE_MAX_FILENAME_LENGTH);
	while(LockHandleAcquire(fds_lock) != SYNC_SUCCESS);
	fds[handle].valid = 0;
	LockHandleRelease(fds_lock);
	return FILE_SUCCESS;
}

// read num_bytes from the open file descriptor identified by handle. 
// Return FILE_FAIL on failure or upon reaching end of file, and the number of bytes read on success. 
// If end of file is reached, the end-of-file flag in the file descriptor should be set.
int FileRead(int handle, void *mem, int num_bytes) {
	int r_bytes;
	int f_size;
	if(!fds[handle].valid) {
		return FILE_FAIL;
	}
	if(!FILE_CHECK_FLAG_R(fds[handle].flag)) {
		return FILE_FAIL;
	}
	f_size = DfsInodeFilesize(fds[handle].inode_handle);
	if(fds[handle].cur_pos + num_bytes >= f_size) {
		// we are going to read too much than the file size
		num_bytes = f_size - fds[handle].cur_pos;
	}
	r_bytes = DfsInodeReadBytes(fds[handle].inode_handle, mem, fds[handle].cur_pos, num_bytes);
	if(r_bytes == DFS_FAIL) {
		dbprintf('a', "FileRead: DfsInodeReadBytes failure\n");
		return FILE_FAIL;
	}
	
	fds[handle].cur_pos = fds[handle].cur_pos + r_bytes;
	if(fds[handle].cur_pos >= f_size) {
		fds[handle].flag |= FILE_FLAG_EOF;
		return FILE_EOF;
	}

	return r_bytes;
}

// write num_bytes to the open file descriptor identified by handle. 
// Return FILE_FAIL on failure, and the number of bytes written on success.
int FileWrite(int handle, void *mem, int num_bytes) {
	int w_bytes;
	if(!fds[handle].valid) {
		return FILE_FAIL;
	}
	if(!FILE_CHECK_FLAG_W(fds[handle].flag)) {
		return FILE_FAIL;
	}

	w_bytes = DfsInodeWriteBytes(fds[handle].inode_handle, mem, fds[handle].cur_pos, num_bytes);
	if(w_bytes == DFS_FAIL) {
		dbprintf('a', "FileWrite: DfsInodeReadBytes failure\n");
		return DFS_FAIL;
	}
	fds[handle].cur_pos = fds[handle].cur_pos + w_bytes;

	return w_bytes;
}

// seek num_bytes within the file descriptor identified by handle, from the location specified by from_where.
// There are three possible values for from_where: FILE_SEEK_CUR (seek relative to the current position), 
// FILE_SEEK_SET (seek relative to the beginning of the file), 
// and FILE_SEEK_END (seek relative to the end of the file). Any seek operation will clear the eof flag.
int FileSeek(int handle, int num_bytes, int from_where) {
	if(!fds[handle].valid) {
		return FILE_FAIL;
	}
	if(from_where == FILE_SEEK_CUR) {
		fds[handle].cur_pos = fds[handle].cur_pos + num_bytes;
	}
	else if(from_where == FILE_SEEK_SET) {
		fds[handle].cur_pos = num_bytes;
	}
	else if(from_where == FILE_SEEK_END) {
		fds[handle].cur_pos = DfsInodeFilesize(fds[handle].inode_handle) + num_bytes;
	}

	fds[handle].flag = fds[handle].flag & (FILE_FLAG_EOF ^ 0xFF);
	return FILE_SUCCESS;
}

// delete the file specified by filename. Return FILE_FAIL on failure, and FILE_SUCCESS on success.
int FileDelete(char *filename) {
	int inode_handle = DfsInodeFilenameExists(filename);
	if(inode_handle == DFS_FAIL) {
		return FILE_FAIL;
	}
	if(DfsInodeDelete(inode_handle) == DFS_FAIL) {
		return FILE_FAIL;
	}
	return FILE_SUCCESS;
}

// Link a file to a new name in the file system. srcpath is the name of an existing file 
// and dstpath is the new (additional) name the file will have after the link call is completed. 
// A process should be able to access a file through any of the path name created explicitly or through the link call.
// If the srcpath is a directory, then return -1. On all errors, return -1. On success, return 0.
// Example usage: result = link("/a/foo","/newfoo");
int FileLink(char *srcpath, char *dstpath) {
	return FILE_SUCCESS;
}

// Create the specified directory. All directories leading to the leaf must be executable.
// The parent directory must additionally be writable and the leaf should not already exist.
// Return FILE_FAIL on failure, and FILE_SUCCESS on success.
int MkDir(char *path, int permissions) {
	return FILE_SUCCESS;
}

// This function behaves identical to FileDelete() except on a directory.
// Additionally, the leaf must be empty. (eg, all directory entries must be '\0'.) 
// If called on the root directory, this function should fail. If the leaf is not empty, 
// this function fails. Return FILE_FAIL on failure, and FILE_SUCCESS on success.
int RmDir(char *path) {
	return FILE_SUCCESS;
}