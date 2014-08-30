typedef struct node {
  long value;
  struct node *next;
} Node;

typedef struct list {
  Node *node;
  struct list *next;
} List;

Node* Load_File(char *Filename);
int Save_File(char *Filename, Node* list);
void Shell_Sort(Node *list);
