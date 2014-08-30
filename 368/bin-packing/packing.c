#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define HCUT 'H'
#define VCUT 'V'
#define LEAF '-'

typedef struct _node {
  // int thisnode;	
  int parnode;		// noden of right child - real rect number
  int left;		// noden of left child - real rect number
  int right;		// noden of right child - real rect number
  char cutline;		// H, V or -
  double width;		// w & h
  double height;
  double xcoord;	// x & y
  double ycoord;
} Node;

Node* LoadFile(char* filename, int* treelen, int* numrecs) {
  FILE* fin = fopen(filename, "r");
  if(fin == NULL) {
    printf("Error when opening file to read\n");
    return NULL;
  }

  fscanf(fin, "%d", numrecs);
  fscanf(fin, "%d", treelen);

  Node* tree = malloc(sizeof(Node) * (*treelen));
  int i;
  // Assuming that input nodes will be in ascending order
  for(i = 0; i < (*numrecs); i++) {
    // 'thisnode' property will be skipped because 
    // by assumption it can be derived from the array index 
    fscanf(fin, "%*d %d %d %d %c %le %le", 
          &(tree[i].parnode), &(tree[i].left), &(tree[i].right), 
          &(tree[i].cutline), &(tree[i].width), &(tree[i].height));
  }

  for( ; i < (*treelen); i++) {
    fscanf(fin, "%*d %d %d %d %c %*c %*c", &(tree[i].parnode), 
          &(tree[i].left), &(tree[i].right), &(tree[i].cutline)); 
  }
	fclose(fin);
  return tree;
}

int SaveFile(char* filename, Node* tree, int numrecs) {
  FILE* fout = fopen(filename, "w");
  if(fout == NULL) {
    printf("Error when opening file to write\n");
    return EXIT_FAILURE;
  }
  fprintf(fout, "%d\n", numrecs);
  int n;
  for(n = 1; n <= numrecs; n++) {
    fprintf(fout, "%d %le %le %le %le\n", n, 
        tree[n-1].width, tree[n-1].height, 
        tree[n-1].xcoord, tree[n-1].ycoord);
  }
  free(tree);
	fclose(fout);
  return EXIT_SUCCESS;
}

// Get node from `thisnode` number 
#define GETN(tree, n) (tree[(n) - 1])
#define RIGHTC(tree, n) (tree[tree[(n) - 1].right - 1])
#define LEFTC(tree, n) (tree[tree[(n) - 1].left - 1])

// Helper macros
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define SUM(x,y) ((x) + (y))

// Set min height and width of Boxes
void Boxer(Node* tree, int n) {
  if(GETN(tree, n).cutline == '-') {
    return;
  }
  Boxer(tree, GETN(tree, n).left);
  Boxer(tree, GETN(tree, n).right);
  if(GETN(tree, n).cutline == 'V') {
    GETN(tree, n).width = SUM(RIGHTC(tree, n).width, LEFTC(tree, n).width);
    GETN(tree, n).height = MAX(RIGHTC(tree, n).height, LEFTC(tree, n).height);
  }
  else {
    GETN(tree, n).height = SUM(RIGHTC(tree, n).height, LEFTC(tree, n).height);
    GETN(tree, n).width = MAX(RIGHTC(tree, n).width, LEFTC(tree, n).width);
  }
  return;
}

// Set coords of Boxes
void Cooder(Node* tree, int n) {
  if(GETN(tree, n).cutline == '-') {
    return;
  }
  else if(GETN(tree, n).cutline == 'H') {
    LEFTC(tree, n).xcoord = GETN(tree, n).xcoord;
    LEFTC(tree, n).ycoord = RIGHTC(tree, n).height + GETN(tree, n).ycoord;
    RIGHTC(tree, n).ycoord = GETN(tree, n).ycoord;
    RIGHTC(tree, n).xcoord = GETN(tree, n).xcoord;
  }
  else {
    RIGHTC(tree, n).ycoord = GETN(tree, n).ycoord;
    RIGHTC(tree, n).xcoord = LEFTC(tree, n).width + GETN(tree, n).xcoord;
    LEFTC(tree, n).ycoord = GETN(tree, n).ycoord;
    LEFTC(tree, n).xcoord = GETN(tree, n).xcoord;
  }

  Cooder(tree, GETN(tree, n).left);
  Cooder(tree, GETN(tree, n).right);
  return;
}

void Pack(Node* tree, int root) {
  Boxer(tree, root);
  Cooder(tree, root);
}

int FindRoot(Node* tree, int length) {
  int i = length;
  while(GETN(tree, i).parnode != -1) {
    i = GETN(tree, i).parnode;    
  }
  return i;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    printf("Not enough arguments.\n");
    return EXIT_FAILURE;
  }
  int numrecs;
  int length;
  Node* tree = LoadFile(argv[1], &length, &numrecs);
  if(tree == NULL) {
    return EXIT_FAILURE;
  }
  clock_t pack_start = clock();
  int root = FindRoot(tree, length);
  Pack(tree, root);
  clock_t pack_end = clock();
  double packtime = (double) (pack_end - pack_start) / CLOCKS_PER_SEC;

  printf("Width: %le\n", GETN(tree, root).width);
  printf("Height: %le\n\n", GETN(tree, root).height);
  printf("X-coordinate: %le\n", GETN(tree, numrecs).xcoord);
  printf("Y-coordinate: %le\n\n", GETN(tree, numrecs).ycoord);
  printf("Elapsed time: %le\n", packtime);
  printf("X %le Y: %le\n", GETN(tree, root).xcoord, GETN(tree, root).ycoord);

  int error_code = SaveFile(argv[2], tree, numrecs);
  return error_code;
}
