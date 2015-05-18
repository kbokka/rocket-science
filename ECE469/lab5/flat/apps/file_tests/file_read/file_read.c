#include "usertraps.h"
#include "misc.h"

#include "file_tests.h"

void main (int argc, char *argv[])
{
  unsigned int i;
  unsigned int ii;
	int fhandle;
  int temp;
  char bunch[] = "bunch of stuff to write on file";
  char buffer[FILE_MAX_READWRITE_BYTES];

  Printf("------------Starting file_read Test 1\n");
  fhandle = file_open("testfile.txt", "r");
  if(fhandle == FILE_FAIL) {
  	Printf("Couldn't open file\n");
  	return;
  }

  temp = file_read(fhandle, buffer, FILE_MAX_READWRITE_BYTES);
  if(temp == FILE_EOF) {
  	Printf("[PASSED] Correctly reached end of file\n");
  }
  else {
  	Printf("[FAIL] Didn't reach end of file\n");
  }
  
  if(dstrncmp(buffer, bunch, FILE_MAX_READWRITE_BYTES) == 0) {
  	Printf("[PASSED] Correctly read buffer: \"%s\"\n", buffer);
  }
  else {
  	Printf("[FAIL] read buffer:\"%s\"\n", buffer);
  }
  file_close(fhandle);
  Printf("------------Starting file_read Test 2\n");
  fhandle = file_open("testfile2.txt", "r");
  if(fhandle == FILE_FAIL) {
    Printf("Couldn't open file\n");
    return;
  }
  Printf("Going to read from 0 to 1500\n");
  // 1500 * 4
  for(i = 0; i < 1500; i++) {
    temp = file_read(fhandle, &ii, sizeof(i));
    if(ii != i)  {
      Printf("ERROR %d instead of %d!\n", ii, i);
      break;
    }
  }
  if(i == 1500) {
    Printf("[PASSED] Wrote (1500*4)=6000 bytes > 5120 bytes\n");
  }
  file_close(fhandle);
  
  file_delete("testfile.txt");
  file_delete("testfile2.txt");

  Printf("------------Ending file_read\n");
}
