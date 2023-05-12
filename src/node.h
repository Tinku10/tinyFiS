#ifndef FS_NODE_H
#define FS_NODE_H

#include <sys/types.h>
#include <time.h>

#define MAX_SUBNODES 100

typedef enum NODE_TYPE {
  REG_FILENODE,
  SOFTLINK_NODE,
  HARDLINK_NODE,
  DIRNODE,
  NULLNODE
} NODE_TYPE;

typedef struct node_info_t {
  struct node_t* children[MAX_SUBNODES];
  char* content;
  unsigned int size;
  struct timespec created_time;
  struct timespec modified_time;
  struct timespec accessed_time;
  // count of hard link
  unsigned int link_count;
  // path to the node for soft link
  char* soft_link_target;
  // pointer to hard link
  node_t* hard_link_target;
} node_info_t;

typedef struct node_t {
  char* name;
  struct node_t* parent;
  NODE_TYPE type;
  // max nodes inside a dir node
  /* struct node_t* children[MAX_SUBNODES]; */
  int node_idx;
  /* char* content; */
  /* unsigned int size; */
  node_info_t* info;
} node_t;

node_t* node_init(char* name);
void node_destroy(node_t* node);
node_t* node_get(node_t* node, char* name);

// directory functions
void dir_add(node_t* node, node_t* child);
void dir_remove(node_t* node);
void dir_set_next_idx(node_t* node);

// regular file functions
void file_add(node_t* node, node_t* child);
void file_write(node_t* node, const char* content, off_t offset, size_t size);
char* file_read(node_t* node, off_t offset);
int file_truncate(node_t* node, off_t offset);
void file_remove(node_t* node);
void file_link(node_t* node, node_t* link);

#endif
