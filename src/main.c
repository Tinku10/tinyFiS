#include <stdio.h>

#include "fs.h"
#include <assert.h>

static filesystem_t* fs = NULL;

static int do_getattr(const char* path, struct stat* st) {
  printf("getting attribute\n");
  return fs_getattr(fs, path, st);
}

static int do_readdir(const char* path,
                      void* buffer,
                      fuse_fill_dir_t filler,
                      off_t offset,
                      struct fuse_file_info* fi) {
  printf("reading a directory\n");
  return fs_readdir(fs, path, buffer, filler, offset, fi);
}

static int do_read(const char* path,
                   char* buffer,
                   size_t size,
                   off_t offset,
                   struct fuse_file_info* fi) {
  return fs_read(fs, path, buffer, size, offset, fi);
}

static int do_mkdir(const char* path, mode_t mode) {
  printf("making a directory\n");
  return fs_mkdir(fs, path, mode);
}

static int do_mknod(const char* path, mode_t mode, dev_t dev) {
  return fs_mknod(fs, path, mode, dev);
}

static int do_write(const char* path,
                    const char* buffer,
                    size_t size,
                    off_t offset,
                    struct fuse_file_info* info) {
  printf("writing to file\n");
  return fs_write(fs, path, buffer, size, offset, info);
}

static int do_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
  return fs_create(fs, path, mode, fi);
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .mknod = do_mknod,
    .mkdir = do_mkdir,
    .read = do_read,
    .write = do_write,
    .readdir = do_readdir,
    .create = do_create,
};

int main(int argc, char* argv[]) {
  printf("starting the file system\n");

  fs = (filesystem_t*)malloc(sizeof(filesystem_t));

  assert(fs != NULL);

  fs_init(fs);

  return fuse_main(argc, argv, &operations, NULL);
}
