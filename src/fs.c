#include "fs.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void fs_init(filesystem_t* fs) {
  char* root_name = "/";
  fs->root = dir_init(NULL, root_name);
  fs->curr_node = fs->root;
}

node_t* fs_get_last_node(filesystem_t* fs, const char* path) {
  printf("finding last node in %s\n", path);

  if (strcmp(path, fs->root->name) == 0) return fs->root;

  // path always starts from root of the file system
  fs->curr_node = fs->root;

  char* path_copy = strdup(path);
  char* curr_token = strtok(path_copy, "/");

  while (curr_token != NULL) {
    node_t* prev = fs->curr_node;
    fs->curr_node = node_get(fs->curr_node, curr_token);
    char* next_token = strtok(NULL, "/");

    // if there is no node, create a empty node and return
    if (fs->curr_node == NULL) {
      // if the current token does not exist in the system
      // it cannot have a child node
      if (next_token != NULL) {
        printf("%s does not exist", curr_token);
        return NULL;
      }

      fs->curr_node = node_init(curr_token);
      fs->curr_node->parent = prev;

      break;
    }

    curr_token = next_token;
  }

  free(path_copy);

  return fs->curr_node;
}

int fs_getattr(filesystem_t* fs, const char* path, struct stat* st) {
  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) return -ENOENT;

  if (base->type == DIRNODE) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if (base->type == FILENODE) {
    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = base->size;
  } else {
    return -ENOENT;
  }

  return 0;
}

int fs_readdir(filesystem_t* fs,
               const char* path,
               void* buffer,
               fuse_fill_dir_t filler,
               off_t offset,
               struct fuse_file_info* fi) {
  printf("reading directory %s\n", path);

  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) return -1;

  for (int i = 0; i < MAX_SUBNODES; i++) {
    if (base->children[i] == NULL) break;

    filler(buffer, base->children[i]->name, NULL, 0);
  }

  return 0;
}

int fs_read(filesystem_t* fs,
            const char* path,
            char* buffer,
            size_t size,
            off_t offset,
            struct fuse_file_info* fi) {
  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) return -1;

  int bytes_to_read = base->size - offset;

  if (bytes_to_read > size) bytes_to_read = size;

  printf("reading from %s of size %d bytes at offset %lld\n", path, bytes_to_read, offset);

  char* content = file_read(base, offset);
  memcpy(buffer, content, bytes_to_read);

  return size - offset;
}

int fs_mkdir(filesystem_t* fs, const char* path, mode_t mode) {
  node_t* base = fs_get_last_node(fs, path);
  node_t* parent = base->parent;

  if (base == NULL) return -1;

  dir_add(parent, base);

  return 0;
}

int fs_mknod(filesystem_t* fs, const char* path, mode_t mode, dev_t dev) {
  node_t* base = fs_get_last_node(fs, path);
  node_t* parent = base->parent;

  if (base == NULL) return -1;

  file_add(parent, base);

  return 0;
}

int fs_write(filesystem_t* fs,
             const char* path,
             const char* buffer,
             size_t size,
             off_t offset,
             struct fuse_file_info* info) {
  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) return -1;

  printf("writing %s of size %d bytes to %s\n", buffer, size, base->name);

  file_write(base, buffer, offset, size);

  return size;
}

int fs_create(filesystem_t* fs, const char* path, mode_t mode, struct fuse_file_info* fi) {
  node_t* base = fs_get_last_node(fs, path);
  node_t* parent = base->parent;

  if (base == NULL) return -1;

  file_add(parent, base);

  return 0;
}
