#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sorting.h"

int main(int argc, char* argv[]) {
  if(argc != 3) {
    printf("Incorrect arguments\n");
    exit(EXIT_FAILURE);
  }

  clock_t io_start;
  clock_t io_end;
  clock_t io_time;
  clock_t sort_start;
  clock_t sort_end;
  clock_t sort_time;

  Node* list = NULL;
  io_start = clock();
  list = Load_File(argv[1]);
  io_end = clock();
  io_time = io_end - io_start;

  if(list == NULL) {
    exit(EXIT_FAILURE);
  }
  sort_start = clock();
  Shell_Sort(list);       
  sort_end = clock();
  sort_time = sort_end - sort_start;

  io_start = clock();
  Save_File(argv[2], list);
  io_end = clock();
  io_time += (io_end - io_start);

  double iotime = (double) io_time / CLOCKS_PER_SEC;
  double sorttime = (double) sort_time / CLOCKS_PER_SEC;

  // Report
  printf("I/O time: %le\n", iotime);
  printf("Sorting time: %le\n", sorttime);

  return 0;
}
