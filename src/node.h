#include <stddef.h>

typedef enum NODE_TYPE { FILENODE, DIRNODE } NODE_TYPE;

#define MX_SUBNODES 100

typedef struct node_t {
  char* name;
  struct node_t* parent;
  NODE_TYPE type;
  // max nodes inside a dir node
  struct node_t* children[MX_SUBNODES];
  int node_idx;
  char* content;
  unsigned int size;
} node_t;

node_t* get_node(node_t* node, char* name);

node_t* dir_init(node_t* node, char* name);

node_t* file_init(node_t* node, char* name);
void file_write(node_t* node, const char* content, off_t offset, size_t size);
char* file_read(node_t* node);
