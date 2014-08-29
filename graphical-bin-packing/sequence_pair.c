#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _node {
  struct _node* next;
  int rectnum; // rectangle number 
} Node;

typedef struct _box {
  double width;
  double height;
} Box;

typedef struct _vertex {
  Node* rights;    // adjacency list for HCG
  Node* aboves;    // adjacency list for VCG
  double width;	  // w & h
  double height;
  double xcoord;  // x & y
  double ycoord;
  char color;
  int seq1_index;
  int seq2_index;
} Vertex;

Node* Node_construct(Node* next, int rectnum) {
  Node* n = malloc(sizeof(Node));
  n->rectnum= rectnum;
  n->next = next;
  return n;
}

Node* List_prepend(Node* head, int rectnum) {
  return Node_construct(head, rectnum);
}

void List_destroy(Node* n) {
  Node* prev;
  while(n != NULL) {
    prev = n;
    n = n->next;
    free(prev);
    prev = NULL;
  }
}

void DestroyGraph(Vertex* v, int numrecs) {
  int i;
  for(i = 0; i < numrecs; i++) {
    List_destroy(v[i].rights);
    List_destroy(v[i].aboves);
  }
  free(v);
  v = NULL;
}

// Get vertex by rectangle number
#define GETV(vertices, num) (vertices[(num) - 1])

Vertex* LoadFile(char* filename, int* numrecs) {
  FILE* fin = fopen(filename, "r");
  if(fin == NULL) {
    printf("Error when opening file to read\n");
    return NULL;
  }

  fscanf(fin, "%d", numrecs);

  Vertex* vertices = malloc(sizeof(Vertex) * (*numrecs));
  int i;
  for(i = 0; i < (*numrecs); i++) {
    // `i` index maps to `i+1` rectangle
    fscanf(fin, "%*d %le %le", &(vertices[i].width), &(vertices[i].height));
    vertices[i].rights = NULL;
    vertices[i].aboves = NULL;
    vertices[i].xcoord = 0.0;
    vertices[i].ycoord = 0.0;
    vertices[i].color = 'W';
  }

  int rnum; // rectanlge number
  for(i = 0; i < (*numrecs); i++) {
    fscanf(fin, "%d", &rnum);
    GETV(vertices, rnum).seq1_index = i;
  }
  for(i = 0; i < (*numrecs); i++) {
    fscanf(fin, "%d", &rnum);
    GETV(vertices, rnum).seq2_index = i;
  }

  fclose(fin);
  return vertices;
}

int SaveFile(char* filename, Vertex* vertices, int numrecs) {
  FILE* fout = fopen(filename, "w");
  if(fout == NULL) {
    printf("Error when opening file to write\n");
    return EXIT_FAILURE;
  }

  fprintf(fout, "%d\n", numrecs);
  int i;
  for(i = 0; i < numrecs; i++) {
    // index `i` maps to `i+1` rectangle number
    fprintf(fout, "%d %le %le %le %le\n", i+1, 
        vertices[i].width, vertices[i].height, 
        vertices[i].xcoord, vertices[i].ycoord);
  }
  fclose(fout);
  DestroyGraph(vertices, numrecs); 
  return EXIT_SUCCESS;
}

void MakeGraph(Vertex* v, int numrecs) {
  int ns; // rectangle number for source
  int nx; // rectangle number for next
  for(ns = 1; ns <= numrecs; ns++) {
    int s1 = GETV(v, ns).seq1_index;
    int s2 = GETV(v, ns).seq2_index;
    for(nx = ns + 1; nx <= numrecs; nx++) {
      int x1 = GETV(v, nx).seq1_index;
      int x2 = GETV(v, nx).seq2_index;
      // s1 | s2 can nver be equal to x1 | x2
      if(s1 < x1) {
        if(s2 < x2) {
          // <S is left of X> 
          GETV(v, ns).rights = List_prepend(GETV(v, ns).rights, nx);
        }
        else { // s2 > x2
          // <X is below S>
          GETV(v, nx).aboves = List_prepend(GETV(v, nx).aboves, ns);
        }
      }
      else { // s1 > x1
        if(s2 > x2) {
          // <X is left of S> 
          GETV(v, nx).rights = List_prepend(GETV(v, nx).rights, ns); 
        }
        else { // s2 < x2
          // <S is below X>
          GETV(v, ns).aboves = List_prepend(GETV(v, ns).aboves, nx); 
        }
      }
    }
  }
}

// toposortHCG => Modified DFS
// Vertex* v - the array of vertices of the graph
// int num - rectangle number of vertex that needs to search
// int* sorted_out - where to store toposorted
// int pos - where to place the next sorted_out
// return int - next position
int toposortHCG(Vertex *v, int num, int* sorted_out, int pos) {
  GETV(v, num).color = 'G';
  Node* n = GETV(v, num).rights;
  while(n != NULL) {
    if(GETV(v, n->rectnum).color == 'W') {
      pos = toposortHCG(v, n->rectnum, sorted_out, pos);
    }
    n = n->next;
  }
  // At the end of this function all nodes will be B
  GETV(v, num).color = 'B';
  sorted_out[pos] = num;
  return pos - 1;
}

void ToposortHCG(Vertex* v, int numrecs, int* sorted_output) {
  int i;
  int pos = numrecs - 1;
  for(i = 0; i < numrecs; i++) {
    if(v[i].color == 'W') {
      pos = toposortHCG(v, i + 1, sorted_output, pos);
    }
  }
}

// int* tsorted - has the recnumber in topological order
void PackHCG(Vertex* v, int numrecs, int* tsorted, Box* sink) {
  int i;
  for(i = 0; i < numrecs; i++) {
    double xcoord = GETV(v, tsorted[i]).xcoord;
    double width = GETV(v, tsorted[i]).width;
    double trouble = xcoord + width;
    Node* n = GETV(v, tsorted[i]).rights;
    while(n != NULL) {
      if(GETV(v, n->rectnum).xcoord < trouble) {
        GETV(v, n->rectnum).xcoord = trouble;
      }
      n = n->next;
    }
    if(sink->width < trouble) {
      sink->width = trouble;
    }
  }
}

// toposortVCG => Modified DFS - from black to white
// Vertex* v - the array of vertices of the graph
// int num - rectangle number of vertex that needs to search
// int* sorted_out - where to store toposorted
// int pos - where to place the next sorted_out
// return int - next position
int toposortVCG(Vertex *v, int num, int* sorted_out, int pos) {
  GETV(v, num).color = 'G';
  Node* n = GETV(v, num).aboves;
  while(n != NULL) {
    // From B to W because HCG already converted from W to B 
    if(GETV(v, n->rectnum).color == 'B') {
      pos = toposortVCG(v, n->rectnum, sorted_out, pos);
    }
    n = n->next;
  }
  GETV(v, num).color = 'W';
  sorted_out[pos] = num;
  return pos - 1;
}

void ToposortVCG(Vertex* v, int numrecs, int* sorted_output) {
  int i;
  int pos = numrecs - 1;
  for(i = 0; i < numrecs; i++) {
    if(v[i].color == 'B') {
      pos = toposortVCG(v, i + 1, sorted_output, pos);
    }
  }
}

// int* tsorted - has the recnumber in topological order
void PackVCG(Vertex* v, int numrecs, int* tsorted, Box* sink) {
  int i;
  for(i = 0; i < numrecs; i++) {
    double ycoord = GETV(v, tsorted[i]).ycoord;
    double height = GETV(v, tsorted[i]).height;
    double trouble = ycoord + height;
    Node* n = GETV(v, tsorted[i]).aboves;
    while(n != NULL) {
      if(GETV(v, n->rectnum).ycoord < trouble) {
        GETV(v, n->rectnum).ycoord = trouble;
      }
      n = n->next;
    }
    if(sink->height < trouble) {
      sink->height = trouble;
    }
  }
}


Box Pack(Vertex* v, int numrecs) {
  Box sink = {0.0, 0.0};
  int* tsorted_recs = malloc(sizeof(int) * numrecs);
  MakeGraph(v, numrecs);
  // Toposort HCG
  ToposortHCG(v, numrecs, tsorted_recs);
  // Pack HCG
  PackHCG(v, numrecs, tsorted_recs, &sink);
  // Toposort VCG
  ToposortVCG(v, numrecs, tsorted_recs);
  // Pack VCG
  PackVCG(v, numrecs, tsorted_recs, &sink);
  free(tsorted_recs);
  return sink;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    printf("Not enough arguments.\n");
    return EXIT_FAILURE;
  }
  int numrecs;
  Vertex* vertices = LoadFile(argv[1], &numrecs);

  clock_t pack_start = clock();
  Box bounds = Pack(vertices, numrecs);
  clock_t pack_end = clock();
  double packtime = ((double) (pack_end - pack_start)) / CLOCKS_PER_SEC;

  printf("Width: %le\n", bounds.width);
  printf("Height: %le\n\n", bounds.height);
  printf("X-coordinate: %le\n", GETV(vertices, numrecs).xcoord);
  printf("Y-coordinate: %le\n\n", GETV(vertices, numrecs).ycoord);
  printf("Elapsed Time: %le\n", packtime);

  SaveFile(argv[2], vertices, numrecs);
  return 0;
}
