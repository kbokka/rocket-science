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

#endif
