#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int * array = malloc(512);
  int * array2;
  int * array3;
  int * array4;
  int i;
  int size;
  Printf("***** Test 1: Malloc for 512 bytes *****\n");
  for (i = 0; i < (512 / 4); i++)
  {
    array[i] = i;
  }
  for (i = 0; i < (512 / 4); i++)
  {
    Printf("%d\n", array[i]);
  }
  size = mfree(array);
  Printf("Amount freed: %d\n", size);
  Printf("***** Test 2: Mallocing 30 bytes less than order 0 size *****\n");
  array = malloc(30);
  for (i = 0; i < (30 / 4); i++)
  {
    array[i] = i;
  }
  for (i = 0; i < (30 / 4); i++)
  {
    Printf("%d\n", array[i]);
  }
  size = mfree(array);
  Printf("Amount freed: %d\n", size);
  
  Printf("***** Test 3: Mallocing mutliple mem blocks *****\n");
  array = malloc(200);
  Printf("-------------------------------------------------\n");
  array2 = malloc(300);
  Printf("-------------------------------------------------\n");
  array3 = malloc(128);
  Printf("-------------------------------------------------\n");
  array4 = malloc(1024);
  Printf("-------------------------------------------------\n");
  mfree(array);
  mfree(array2);
  mfree(array3);
  mfree(array4);
  Printf("-------------------------------------------------\n");

  Printf("****** Test 4: Mallocing mem block larger than Heap\n");
  array = malloc(4100);
  Printf("Malloc returned %d\n", (int)array);
  mfree(array);

  Printf("PID=%d: xxxxxxxxxx Done xxxxxxxxxx\n", getpid());
}