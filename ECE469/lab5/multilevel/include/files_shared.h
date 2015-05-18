#ifndef __FILES_SHARED__
#define __FILES_SHARED__

#define FILE_SEEK_SET 1
#define FILE_SEEK_END 2
#define FILE_SEEK_CUR 3

#define FILE_MAX_FILENAME_LENGTH 72 // is the same as in dfs_shared.h DFS_FILENAME_LENGTH
#define FILE_MAX_READWRITE_BYTES 4096

typedef struct file_descriptor {
	char valid;
	char filename[FILE_MAX_FILENAME_LENGTH]; // keeping this since it will be useful with step 7
	unsigned int inode_handle;
	unsigned int cur_pos;
	char flag; // MSB[ ... , EOF, w, r ]LSB
} file_descriptor;

#define FILE_FAIL -1
#define FILE_EOF -2
#define FILE_SUCCESS 1

#endif
