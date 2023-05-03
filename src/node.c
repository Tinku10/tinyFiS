#include "node.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

node_t* get_node(node_t* node, char* name) {
  if (node == NULL) return NULL;

  printf("looking for node %s in %s\n", name, node->name);

  for (int i = 0; i < MAX_SUBNODES; i++) {
    if (node->children[i] == NULL) return NULL;

    if (strcmp(name, node->children[i]->name) == 0) {
      printf("found node %s\n", name);
      return node->children[i];
    }
  }

  printf("not found node %s\n", name);
  return NULL;
}

node_t* dir_init(node_t* node, char* name) {
  printf("initializing %s directory\n", name);

  node_t* dir = (node_t*)malloc(sizeof(node_t));

  assert(dir != NULL);

  dir->name = name;
  dir->parent = node;
  dir->type = DIRNODE;
  dir->node_idx = -1;

  if (node != NULL) {
    node->children[++node->node_idx] = dir;
    printf("added %s to %s at index %d\n", name, node->name, node->node_idx);
  }

  node_t* dot = (node_t*)malloc(sizeof(node_t));

  assert(dot != NULL);

  dot->name = ".";
  dot->parent = dir;
  dot->type = DIRNODE;
  dot->node_idx = -1;

  dir->children[++dir->node_idx] = dot;

  node_t* dotdot = (node_t*)malloc(sizeof(node_t));

  assert(dotdot != NULL);

  dotdot->name = "..";
  dotdot->parent = dir;
  dotdot->type = DIRNODE;
  dotdot->node_idx = -1;

  dir->children[++dir->node_idx] = dotdot;

  return dir;
}

node_t* file_init(node_t* node, char* name) {
  node_t* file = (node_t*)malloc(sizeof(node_t));

  assert(file != NULL);

  file->name = name;
  file->parent = node;
  file->type = FILENODE;
  file->content = NULL;
  file->size = 0;

  if (node != NULL) node->children[++node->node_idx] = file;

  return file;
}

char* file_read(node_t* node) {
  return node->content;
}

void file_write(node_t* node, const char* content, off_t offset, size_t size) {
  if (node->content == NULL)
    node->content = (char*)malloc(size);
  else if (node->size < size)
    node->content = (char*)realloc(node->content, offset + size);

  node->size = offset + size;

  memcpy(node->content + offset, content, size);
}
