#include "fs.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void fs_init(filesystem_t* fs) {
  fs->root = node_init("/");
  dir_add(NULL, fs->root);
  fs->curr_node = fs->root;
}

node_t* fs_get_last_node(filesystem_t* fs, const char* path) {
  printf("finding last node in %s\n", path);

  if (strcmp(path, fs->root->name) == 0) return fs->root;

  // path always starts from root of the file system
  node_t* curr_node = fs->root;

  char* path_copy = strdup(path);
  char* curr_token = strtok(path_copy, "/");

  while (curr_token != NULL) {
    node_t* prev = curr_node;
    curr_node = node_get(curr_node, curr_token);
    char* next_token = strtok(NULL, "/");

    // if there is no node, create a empty node and return
    if (curr_node == NULL) {
      // if the current token does not exist in the system
      // it cannot have a child node
      if (next_token != NULL) {
        printf("%s does not exist\n", curr_token);
        return NULL;
      }

      // curr_token has to be duplicated
      // otherwise when freeing path_copy, name will be a NULL
      curr_node = node_init(strdup(curr_token));
      curr_node->parent = prev;

      break;
    }

    curr_token = next_token;
  }

  free(path_copy);

  return curr_node;
}

int fs_getattr(filesystem_t* fs, const char* path, struct stat* st) {
  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) {
    return -ENOENT;
  }

  if (base->type == DIRNODE) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if (base->type == REG_FILENODE) {
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

  if (base == NULL) {
    printf("directory does not exist");
    return -1;
  }

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

  if (base == NULL) {
    return -1;
  }

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

  if (base == NULL) {
    return -1;
  }

  base->type = DIRNODE;

  printf("adding %s to %s\n", base->name, parent->name);
  dir_add(parent, base);

  return 0;
}

int fs_mknod(filesystem_t* fs, const char* path, mode_t mode, dev_t dev) {
  node_t* base = fs_get_last_node(fs, path);
  node_t* parent = base->parent;

  if (base == NULL) {
    return -1;
  }

  base->type = REG_FILENODE;

  file_add(parent, base);

  return 0;
}

int fs_write(filesystem_t* fs,
             const char* path,
             const char* buffer,
             size_t size,
             off_t offset,
             struct fuse_file_info* fi) {
  node_t* base = fs_get_last_node(fs, path);

  if ((fi->flags & O_ACCMODE) == O_RDONLY) return -EACCES;

  printf("writing %s of size %d bytes to %s\n", buffer, size, base->name);

  file_write(base, buffer, offset, size);

  return size;
}

int fs_create(filesystem_t* fs, const char* path, mode_t mode, struct fuse_file_info* fi) {
  node_t* base = fs_get_last_node(fs, path);
  node_t* parent = base->parent;

  if (base == NULL) {
    return -1;
  }

  base->type = REG_FILENODE;

  file_add(parent, base);

  return 0;
}

int fs_open(filesystem_t* fs, const char* path, struct fuse_file_info* fi) {
  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) {
    return -ENOENT;
  }

  if (base->type != REG_FILENODE) return -EISDIR;

  fi->fh = (intptr_t)base;

  return 0;
}

int fs_release(filesystem_t* fs, const char* path, struct fuse_file_info* fi) {
  node_t* node = (node_t*)(intptr_t)fi->fh;

  if (node == NULL) {
    printf("no file descriptor for %s\n", path);
    return -ENOENT;
  }

  if (node->type != REG_FILENODE) return -EISDIR;

  fi->fh = 0;

  printf("%s closed\n", path);

  return 0;
}

int fs_utimens(filesystem_t* fs, const char* path, const struct timespec tv[2]) {
  node_t* node = fs_get_last_node(fs, path);

  if (node == NULL) {
    return -ENOENT;
  }

  return 0;
}

int fs_truncate(filesystem_t* fs, const char* path, off_t offset) {
  node_t* base = fs_get_last_node(fs, path);

  file_truncate(base, offset);

  return 0;
}

int fs_unlink(filesystem_t* fs, const char* path) {
  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) return -ENOENT;

  file_remove(base);

  return 0;
}

int fs_rmdir(filesystem_t* fs, const char* path) {
  node_t* base = fs_get_last_node(fs, path);

  if (base == NULL) return -ENOENT;

  dir_remove(base);

  return 0;
}

int fs_link(filesystem_t* fs, const char* oldpath, const char* newpath) {
  node_t* oldnode = fs_get_last_node(fs, oldpath);
  node_t* newnode = fs_get_last_node(fs, newpath);

  if (oldnode == NULL) {
    return -ENOENT;
  }

  if (newnode == NULL) {
    return -ENOENT;
  }

  newnode->type = HARDLINK_NODE;

  file_link(oldnode, newnode);

  return 0;
}

int fs_symbolic_link(filesystem_t* fs, const char* target, const char* linkpath) {
  node_t* base = fs_get_last_node(target);
  node_t* newnode = fs_get_last_node(linkpath);

  if (oldnode == NULL) {
    return -ENOENT;
  }

  if (newnode == NULL) {
    return -ENOENT;
  }

  newnode->type = SOFTLINK_NODE;
  newnode->info->soft_link_target = target;
  
  file_link(base, newnode);

  return 0;
}
