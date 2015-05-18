#include "usertraps.h"
#include "misc.h"

#include "file_tests.h"

void main (int argc, char *argv[])
{
	int fhandle;
  int temp;
  int i;
  char bunch[] = "bunch of stuff to write on file";
  int length = dstrlen(bunch);

  Printf("------------Starting file_write part 1\n");
  fhandle = file_open("testfile.txt", "w");
  if(fhandle == FILE_FAIL) {
  	Printf("Couldn't open file\n");
  	return;
  }

  temp = file_write(fhandle, bunch, length);
  if(temp == length) {
  	Printf("[PASSED] Correctly wrote all %d bytes\n", temp);
  }
  else {
  	Printf("[FAIL] Didn't write all %d bytes\n", temp);
  }
  file_close(fhandle);

  Printf("------------Starting file_write part 2\n");
  fhandle = file_open("testfile2.txt", "w");
  if(fhandle == FILE_FAIL) {
    Printf("Couldn't open file\n");
    return;
  }
  Printf("Going to write from 1 to 1500 (which is 6000 bytes and > 5120 direct addressed bytes)\n");
  // 1500 * 4 = 6000 bytes
  for(i = 0; i < 1500; i++) {
    temp = file_write(fhandle, &i, sizeof(i));
  }
  file_close(fhandle);

  Printf("------------Ending file_write\n");
}
