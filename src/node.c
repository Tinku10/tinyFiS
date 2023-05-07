#include "node.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

node_t* node_get(node_t* node, char* name) {
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

void dir_add(node_t* node, node_t* child) {
  printf("initializing %s directory\n", child->name);

  child->parent = node;
  child->type = DIRNODE;
  child->node_idx = -1;

  if (node != NULL) {
    node->children[++node->node_idx] = child;
    printf("added %s to %s at index %d\n", child->name, node->name, node->node_idx);
  }

  // add "." and ".." in a new directory
  node_t* dot = (node_t*)malloc(sizeof(node_t));

  assert(dot != NULL);

  dot->name = ".";
  dot->parent = child;
  dot->type = DIRNODE;
  dot->node_idx = -1;

  child->children[++child->node_idx] = dot;

  node_t* dotdot = (node_t*)malloc(sizeof(node_t));

  assert(dotdot != NULL);

  dotdot->name = "..";
  dotdot->parent = child;
  dotdot->type = DIRNODE;
  dotdot->node_idx = -1;

  child->children[++child->node_idx] = dotdot;
}

node_t* node_init(char* name) {
  printf("creating an empty node %s\n", name);
  node_t* null_node = (node_t*)malloc(sizeof(node_t));

  assert(null_node != NULL);

  null_node->name = name;
  null_node->type = NULLNODE;

  return null_node;
}

void node_destroy(node_t* node) {
  free(node->name);
  free(node);
}

void file_add(node_t* node, node_t* child) {

  child->parent = node;
  child->type = FILENODE;
  child->content = NULL;
  child->size = 0;

  if (node != NULL) node->children[++node->node_idx] = child;
}

char* file_read(node_t* node, off_t offset) {
  return node->content + offset;
}

void file_write(node_t* node, const char* content, off_t offset, size_t size) {
  if (node->content == NULL)
    node->content = (char*)malloc(size);
  else if (node->size < size)
    node->content = (char*)realloc(node->content, offset + size);

  node->size = offset + size;

  memcpy(node->content + offset, content, size);
}

int file_truncate(node_t* node, off_t offset) {
  node->size -= offset;

  memmove(node->content, node->content, offset);
  return 0;
}

