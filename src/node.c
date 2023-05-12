#include "node.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

node_t* node_get(node_t* node, char* name) {
  if (node == NULL) return NULL;

  printf("looking for node %s in %s\n", name, node->name);

  for (int i = 0; i < MAX_SUBNODES; i++) {
    if (node->info->children[i] == NULL) continue;

    if (strcmp(name, node->info->children[i]->name) == 0) {
      printf("found node %s\n", name);
      return node->info->children[i];
    }
  }

  printf("not found node %s\n", name);
  return NULL;
}

void dir_set_next_idx(node_t* node) {
  int next_index = node->node_idx % MAX_SUBNODES;

  int remaining = MAX_SUBNODES;

  while (remaining--) {
    if (node->info->children[next_index] == NULL) {
      node->node_idx = next_index;
      break;
    }
    next_index = (next_index + 1) % MAX_SUBNODES;
  }

}

void dir_add(node_t* node, node_t* child) {
  printf("initializing %s directory\n", child->name);

  child->parent = node;
  child->type = DIRNODE;
  child->node_idx = -1;

  if (node != NULL) {
    dir_set_next_idx(node);
    node->info->children[node->node_idx] = child;
    printf("added %s to %s at index %d\n", child->name, node->name, node->node_idx);
  }

  // add "." and ".." in a new directory
  node_t* dot = (node_t*)malloc(sizeof(node_t));

  assert(dot != NULL);

  dot->name = ".";
  dot->parent = child;
  dot->type = DIRNODE;
  dot->node_idx = -1;

  dir_set_next_idx(child);
  child->info->children[child->node_idx] = dot;

  node_t* dotdot = (node_t*)malloc(sizeof(node_t));

  assert(dotdot != NULL);

  dotdot->name = "..";
  dotdot->parent = child;
  dotdot->type = DIRNODE;
  dotdot->node_idx = -1;

  dir_set_next_idx(child);
  child->info->children[child->node_idx] = dotdot;
}

void dir_remove(node_t* node) {
  node_t* parent = node->parent;

  // remove the parent link
  for (int i = 0; i < MAX_SUBNODES; i++) {
    if (parent->info->children[i] == NULL) continue;

    if (strcmp(node->name, parent->children[i]->name) == 0) {
      parent->info->children[i] = NULL;
      break;
    }
  }

  // destroy all sub-nodes
  if (node->type == DIRNODE) {
    for (int i = 0; i < MAX_SUBNODES; i++) {
      if (node->info->children[i] == NULL) continue;

      node_destroy(node->info->children[i]);
    }
  }

  // destory the node
  node_destroy(node);
}

node_t* node_init(char* name) {
  printf("creating an empty node %s\n", name);
  node_t* null_node = (node_t*)malloc(sizeof(node_t));
  node_info_t* null_node_info = (node_info_t*)malloc(sizeof(node_info_t));

  assert(null_node != NULL);

  null_node->name = name;
  null_node->type = NULLNODE;
  null_node->info = null_node_info;

  return null_node;
}

void node_destroy(node_t* node) {
  free(node->name);
  free(node);
}

void file_add(node_t* node, node_t* child) {
  child->parent = node;
  child->type = REG_FILENODE;
  child->info->content = NULL;
  child->info->size = 0;

  if (node != NULL) node->info->children[++node->node_idx] = child;
}

char* file_read(node_t* node, off_t offset) {
  return node->info->content + offset;
}

void file_write(node_t* node, const char* content, off_t offset, size_t size) {
  if (node->info->content == NULL)
    node->info->content = (char*)malloc(size);
  else if (node->info->size < size)
    node->info->content = (char*)realloc(node->info->content, offset + size);

  node->info->size = offset + size;

  memcpy(node->info->content + offset, content, size);
}

int file_truncate(node_t* node, off_t offset) {
  node->info->size -= offset;

  memmove(node->info->content, node->info->content, offset);
  return 0;
}

void file_remove(node_t* node) {
  node_t* parent = node->parent;

  for (int i = 0; i < MAX_SUBNODES; i++) {
    if (parent->info->children[i] == NULL) continue;

    if (strcmp(node->name, parent->info->children[i]->name) == 0) {
      parent->info->children[i] = NULL;
      break;
    }
  }

  if (node->type == HARDLINK_NODE) {
    node_t* origin = node->info->hard_link_target;

    origin->info->link_count--;

    if (origin->info->link_count == 0) {
      file_remove(origin);
    }
  } 

  // destory the node
  node_destroy(node);
}

void file_link(node_t* node, node_t* link) {
  node_t* parent = node->parent;

  link->parent = node->parent;

  if (link->type == HARDLINK_NODE) {
    link->info->size = node->info->size;
    link->info->content = node->info->content;
    link->info->hard_link_target = node;
  } 
}
