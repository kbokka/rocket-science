#include <stdio.h>
#include <stdlib.h>
#include "sorting.h"

Node* Node_init(long value) {
  Node* node = (Node*) malloc(sizeof(Node));
  node->value = value;
  node->next = NULL;
  return node;
}

Node* Load_File(char *Filename) {
  FILE *fin = fopen(Filename, "r");
  if(fin == NULL) {
    printf("Error opening file\n");
    return NULL;
  }

  Node* list = Node_init(0); // Dummy header
  Node* curr = list;
  int count = 0;

  long value;
  while(fscanf(fin, "%li", &value) == 1) {
    curr->next = Node_init(value);
    curr = curr->next;
    count += 1;
  }

  list->value = count;
  fclose(fin);
  return list;
}

int Save_File(char *Filename, Node* list) {
  FILE *fout = fopen(Filename, "w");
  if(fout == NULL) {
    return 0;
  }

  Node* curr = list->next;
  Node* t_curr = NULL; // temp curr

  int count = 0;
  while(curr != NULL) {
    fprintf(fout, "%ld\n", curr->value);
    t_curr = curr;
    curr = curr->next;
    free(t_curr);
    count += 1;
  }

  free(list); // free the dummy
  fclose(fout);
  return count; // For no error
}

int initSeq(long size) {
  int k = 1;
  // 'tk' will have the last value of k before breaking loop
  // and can be used to check for overflow of k
  int tk = 0;   

  while(k < size && tk < k) {
    tk = k;
    k = k << 1;
    k++;
  }
  return tk;
}

#define genSeq(k) (( (k) - 1 ) >> 1) 

Node* Skip_Links(Node* node, int k) {
  while(k--) {
    node = node->next;
    if(node == NULL) { 
      return NULL;
    }
  }
  return node;
}

void Shell_Sort(Node* list) {
  // list is dummy node
  int size = (int) list->value;
  if(size == 1 || size == 0) { 
    return;
  }
  int k = initSeq(size); // sequence number k
  Node* curr = list->next;
  Node* skipped = curr;
  do {
    int count = (size / k) + 1;
    int i;
    for(i = 0; i < count; i++) {
      curr = list->next;
      skipped = Skip_Links(curr, k); // Get the first skip
      int j;
      for(j = 0; j < (size - k - (i * k)) && skipped != NULL; j++) {
        if (curr->value > skipped->value)
        {
          long temp = skipped->value;
          skipped->value = curr->value;
          curr->value = temp;
        }
        curr = curr->next; 
        skipped = skipped->next; // Continue from last skip
      }
    }
  } while ((k = genSeq(k)) >= 1);
  return;
}
