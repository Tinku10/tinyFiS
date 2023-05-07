#ifndef FS_FS_H
#define FS_FS_H

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <sys/types.h>

#include "node.h"

typedef struct filesystem_t {
  node_t* root;
  node_t* curr_node;
} filesystem_t;

void fs_init(filesystem_t* fs);
node_t* fs_get_last_node(filesystem_t* fs, const char* path);

int fs_getattr(filesystem_t* fs, const char* path, struct stat* st);
int fs_readdir(filesystem_t* fs,
               const char* path,
               void* buffer,
               fuse_fill_dir_t filler,
               off_t offset,
               struct fuse_file_info* fi);
int fs_read(filesystem_t* fs,
            const char* path,
            char* buffer,
            size_t size,
            off_t offset,
            struct fuse_file_info* fi);
int fs_mkdir(filesystem_t* fs, const char* path, mode_t mode);
int fs_mknod(filesystem_t* fs, const char* path, mode_t mode, dev_t dev);
int fs_write(filesystem_t* fs,
             const char* path,
             const char* buffer,
             size_t size,
             off_t offset,
             struct fuse_file_info* info);
int fs_create(filesystem_t* fs, const char* path, mode_t mode, struct fuse_file_info* fi);
int fs_open(filesystem_t* fs, const char* path, struct fuse_file_info* fi);
int fs_release(filesystem_t* fs, const char* path, struct fuse_file_info* fi);
int fs_utimens(filesystem_t* fs, const char* path, const struct timespec tv[2]);
int fs_truncate(filesystem_t* fs, const char* path, off_t offset);

#endif
