#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long *Load_File(char *Filename, int *Size);
int Save_File(char *Filename, long *Array, int Size);
void Shell_Insertion_Sort_Seq1(long *Array, int Size, double *N_Comp, double *N_move);
void Shell_Bubble_Sort_Seq1(long *Array, int Size, double *N_Comp, double *N_move);
void Shell_Insertion_Sort_Seq2(long *Array, int Size, double *N_Comp, double *N_move);
void Shell_Bubble_Sort_Seq2(long *Array, int Size, double *N_Comp, double *N_move);
int Print_Seq_1(char *File_Name, int Size);
int Print_Seq_2(char *File_Name, int Size);

int main(int argc, char* argv[]) {
  if(argc != 6) {
    printf("Incorrect arguments\n");
    exit(EXIT_FAILURE);
  }
  char func = argv[2][0];
  int size;
  double ncomp = 0;
  double nmove = 0;
  
  clock_t io_start;
  clock_t io_end;
  clock_t io_time;
  clock_t sort_start;
  clock_t sort_end;
  clock_t sort_time;


  io_start = clock();
  long *Array = Load_File(argv[3], &size);
  io_end = clock();
  io_time = io_end - io_start;

  if(Array == NULL) {
    exit(EXIT_FAILURE);
  }

  if(argv[1][0] == '1'){
    io_start = clock();
    Print_Seq_1(argv[4], size);
    io_end = clock();
    io_time += (io_end - io_start);

    if(func == 'i') {
      sort_start = clock();
      Shell_Insertion_Sort_Seq1(Array, size, &ncomp, &nmove);       
      sort_end = clock();
      sort_time = sort_end - sort_start;
    }
    else if(func == 'b')  {
      sort_start = clock();
      Shell_Bubble_Sort_Seq1(Array, size, &ncomp, &nmove);       
      sort_end = clock();
      sort_time = sort_end - sort_start;
    }
    else {
      printf("Incorrect arguments\n");
      exit(EXIT_FAILURE);
    }
  }
  else if(argv[1][0] == '2') {
    Print_Seq_2(argv[4], size);
    if(func == 'i') {
      sort_start = clock();
      Shell_Insertion_Sort_Seq2(Array, size, &ncomp, &nmove);       
      sort_end = clock();
      sort_time = sort_end - sort_start;
    }
    else if(func == 'b')  {
      sort_start = clock();
      Shell_Bubble_Sort_Seq2(Array, size, &ncomp, &nmove);       
      sort_end = clock();
      sort_time = sort_end - sort_start;
    }
    else {
      printf("Incorrect arguments\n");
      exit(EXIT_FAILURE);
    }
  }
  else {
    printf("Incorrect arguments\n");
    exit(EXIT_FAILURE);
  }
  
  io_start = clock();
  if(Save_File(argv[5], Array, size) != 0) { // will free the array
    exit(EXIT_FAILURE);
  }
  io_end = clock();
  io_time += (io_end - io_start);

  double iotime = io_time / CLOCKS_PER_SEC;
  double sorttime = sort_time / CLOCKS_PER_SEC;

  // Report
  printf("Number of comparisons: %le\n", ncomp);
  printf("Number of moves: %le\n", nmove);
  printf("I/O time: %le\n", iotime);
  printf("Sorting time: %le\n", sorttime);

  return 0;
}
