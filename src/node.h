#ifndef FS_NODE_H
#define FS_NODE_H

#include <sys/types.h>

#define MAX_SUBNODES 100

typedef enum NODE_TYPE { FILENODE, DIRNODE, NULLNODE } NODE_TYPE;

typedef struct node_t {
  char* name;
  struct node_t* parent;
  NODE_TYPE type;
  // max nodes inside a dir node
  struct node_t* children[MAX_SUBNODES];
  int node_idx;
  char* content;
  unsigned int size;
} node_t;

node_t* node_init(char* name);
void node_destroy(node_t* node);
node_t* node_get(node_t* node, char* name);

void dir_add(node_t* node, node_t* child);
void dir_remove(node_t* node);
void dir_set_next_idx(node_t* node);

void file_add(node_t* node, node_t* child);
void file_write(node_t* node, const char* content, off_t offset, size_t size);
char* file_read(node_t* node, off_t offset);
int file_truncate(node_t* node, off_t offset);
void file_remove(node_t* node);

#endif

