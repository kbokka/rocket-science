#include <stdio.h>
#include <stdlib.h>

long *Load_File(char *Filename, int *Size) {
  FILE *fin = fopen(Filename, "r");
  if(fin == NULL) {
    printf("Error opening file\n");
    return NULL;
  }

  if(fscanf(fin, "%d", Size) < 1) { // First line contains number of integers to be sorted
    exit(EXIT_FAILURE);
  }

  long *data = malloc((*Size) * sizeof(long));

  int count;
  for(count = 0; count < *Size; count++) {
    if(fscanf(fin, "%li", &data[count]) != 1) {
      fclose(fin);
      printf("Error with data in file\n");
      return NULL;
    }
  }

  fclose(fin);
  return data;
}

int Save_File(char *Filename, long *Array, int Size) {
  FILE *fout = fopen(Filename, "w");
  if(fout == NULL) {
    return 0;
  }

  fprintf(fout, "%d\n", Size);  

  int count;
  for(count = 0; count < Size; count++) {
    fprintf(fout, "%ld\n", Array[count]);  
  }

  free(Array);
  fclose(fout);

  return 0; // For no error
}

// -- region _sequence_1 --
int initSeq1(int size) {
  int k = 1;
  // 'tk' will have the last value of k before breaking loop
  // and can be used to check for overflow of k
  int tk = 0;   

  while(k < size && tk < k) {
    tk = k;
    k = 2 * k;
    k++;
  }
  return tk;
}

#define genSeq1(k) (( (k) - 1 ) >> 1) 
// -- endregion _sequence_1 --

// -- region _sequence_2 --
int initSeq2(int size) {
  int k = 1;
  int tk = 0;
  while(tk < size && tk < k) {
    tk = k;
    k = 3 * k;
  }
  return tk;
}

int genSeq2(int k, int *third_base) {
  if ((k &  (k - 1)) == 0) { // if a power of 2
    (*third_base) = (*third_base) / 3;
    return (*third_base);
  }
  // else
  return (k / 3) * 2;
}
// -- endregion _sequence_2 --

void Shell_Insertion_Sort_Seq1(long *Array, int Size, double *N_Comp, double *N_move) {
  int k = initSeq1(Size);        // sequence number k
  int j;
  int i;

  do {
    for(j = k; j < Size; j++) {
      (*N_move)++;
      double temp = Array[j];  
      i = j;
      while(i >= k) {
        (*N_Comp)++;
        if(Array[i-1] > temp) {
          (*N_move)++;
          Array[i] = Array[i-k];
          i = i - k;
        }
        else {
          break;
        }
      }
      (*N_move)++; 
      Array[i] = temp;
    }
  } while ((k = genSeq1(k)) >= 1);
  return;
}

void Shell_Bubble_Sort_Seq1(long *Array, int Size, double *N_Comp, double *N_move) {
  int k = initSeq1(Size);        // sequence number k
  int j;
  int i;
  long temp;

  do {
    for(j = 0; j < Size - k - 1; ++j) {
      for(i =0; i < Size - j - k; ++i) {
        if(Array[i] > Array[i + k]) {
          temp = Array[i + k];
          Array[i + k] = Array[i];
          Array[i] = temp;
        }
      }
    }
  } while ((k = genSeq1(k)) >= 1);
  return;
}

void Shell_Insertion_Sort_Seq2(long *Array, int Size, double *N_Comp, double *N_move) {
  int third_base;
  int k = initSeq2(Size);        // sequence number k
  third_base = k;
  int j;
  int i;

  do {
    for(j = k; j < Size; j++) {
      (*N_move)++;
      long temp = Array[j];  
      i = j;
      while(i >= k) {
        (*N_Comp)++;
        if(Array[i-1] > temp) {
          (*N_move)++;
          Array[i] = Array[i-k];
          i = i - k;
        }
        else {
          break;
        }
      }
      (*N_move)++; 
      Array[i] = temp;
    }
  } while ((k = genSeq2(k, &third_base)) >= 1);
}

void Shell_Bubble_Sort_Seq2(long *Array, int Size, double *N_Comp, double *N_move) {
  int third_base;
  int k = initSeq2(Size);        // sequence number k
  third_base = k;
  int j;
  int i;
  long temp;

  do {
    for(j = 0; j < Size - k - 1; ++j) {
      for(i =0; i < Size - j - k; ++i) {
        (*N_Comp)++;
        if(Array[i] > Array[i + k]) {
          (*N_move) += 3;
          temp = Array[i + k];
          Array[i + k] = Array[i];
          Array[i] = temp;
        }
      }
    }
  } while ((k = genSeq2(k, &third_base)) >= 1);
  return;
}

int Print_Seq_1(char *File_Name, int Size) {
  FILE *fout = fopen(File_Name, "w");
  if(fout == NULL) {
    exit(EXIT_FAILURE);
  }

  fprintf(fout, "%d\n", Size);  

  int k = 1;
  // 'tk' will have the last value of k before breaking loop
  // and can be used to check for overflow of k
  int tk = 0;   
  int count = 0;

  while(k < Size && tk < k) {
    tk = k;
    fprintf(fout, "%d\n", k);  
    count++;
    k = 2 * k;
    k++;
  }

  fclose(fout);
  return count;
}

int ipow(int base, int exp)
{
  int result = 1;
  while (exp) {
    if (exp & 1) {
      result *= base;
    }
    exp >>= 1;
    base *= base;
  }
  return result;
}

int Print_Seq_2(char *File_Name, int Size) {
  FILE *fout = fopen(File_Name, "w");
  if(fout == NULL) {
    exit(EXIT_FAILURE);
  }

  fprintf(fout, "%d\n", Size);  

  int k = 0;
  int p = 0;
  int q;
  int n = 0;
  int counter = 0;

  for(;;) {
    p = n;
    q = 0;
    while(p >= 0) {
      k = ipow(2, p) * ipow(3, q);
      counter++;
      fprintf(fout, "%d\n", k);
      if ((p == 0) && ((k * 3) >= Size)) {
        break;
      }
      q += 1;
      p -= 1;
    }
    if ((p == 0) && ((k * 3) >= Size)) {
      break;
    }
    n += 1;
  }	 
  fclose(fout);
  return counter;
}
