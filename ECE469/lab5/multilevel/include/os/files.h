#ifndef __FILES_H__
#define __FILES_H__

#include "dfs.h"
#include "files_shared.h"

#define FILE_MAX_OPEN_FILES 15

// flag = MSB[ ... , EOF, w, r ]LSB
#define FILE_FLAG_R 	0x1
#define FILE_FLAG_W 	0x2
#define FILE_FLAG_RW 	0x3
#define FILE_FLAG_EOF 	0x4

#define FILE_CHECK_FLAG_R(flag) ((flag) & FILE_FLAG_R)
#define FILE_CHECK_FLAG_W(flag) ((flag) & FILE_FLAG_W)
#define FILE_CHECK_FLAG_RW(flag) ((((flag) & FILE_FLAG_R) || ((flag) & FILE_FLAG_R)) == FILE_FLAG_RW)
#define FILE_CHECK_FLAG_EOF(flag) ((flag) & FILE_FLAG_EOF)

void FileModuleInit();
int FileOpen(char *filename, char *mode);
int FileClose(int handle);
int FileRead(int handle, void *mem, int num_bytes);
int FileWrite(int handle, void *mem, int num_bytes);
int FileSeek(int handle, int num_bytes, int from_where);
int FileDelete(char *filename);

#define INVALID_FILE		-1
#define INVALID_INODE		-1
#define FILE_EXISTS			-2
#define DOES_NOT_EXIST		-3
#define DIRECTORY_EXISTS	-4
#define NOT_A_DIRECTORY		-5
#define NOT_A_FILE			-6
#define INVALID_PATH		-7
#define PERMISSION_DENIED	-8
#define DIRECTORY_NOT_EMPTY	-9

#define FILE_MAX_FILENAME_LENGTH 72
typedef struct file_dir_entry {
  char filename[FILE_MAX_FILENAME_LENGTH];
  int inode_handle;
} file_dir_entry;

#endif