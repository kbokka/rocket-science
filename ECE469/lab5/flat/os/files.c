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