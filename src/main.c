#include <assert.h>
#include <stdio.h>

#include "fs.h"

static filesystem_t* fs = NULL;

static int do_getattr(const char* path, struct stat* st) {
  printf("======getting attribute======\n");
  return fs_getattr(fs, path, st);
}

static int do_readdir(const char* path,
                      void* buffer,
                      fuse_fill_dir_t filler,
                      off_t offset,
                      struct fuse_file_info* fi) {
  printf("======reading directory %s======\n", path);
  return fs_readdir(fs, path, buffer, filler, offset, fi);
}

static int do_read(const char* path,
                   char* buffer,
                   size_t size,
                   off_t offset,
                   struct fuse_file_info* fi) {
  printf("======reading file %s======\n", path);
  return fs_read(fs, path, buffer, size, offset, fi);
}

static int do_mkdir(const char* path, mode_t mode) {
  printf("======making directory %s======\n", path);
  return fs_mkdir(fs, path, mode);
}

static int do_mknod(const char* path, mode_t mode, dev_t dev) {
  printf("======making node %s======\n", path);
  return fs_mknod(fs, path, mode, dev);
}

static int do_write(const char* path,
                    const char* buffer,
                    size_t size,
                    off_t offset,
                    struct fuse_file_info* info) {
  printf("======writing to file %s======\n", path);
  return fs_write(fs, path, buffer, size, offset, info);
}

static int do_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
  printf("======making file %s======\n", path);
  return fs_create(fs, path, mode, fi);
}

static int do_open(const char* path, struct fuse_file_info* fi) {
  printf("======opening file %s======\n", path);
  return fs_open(fs, path, fi);
}

static int do_release(const char* path, struct fuse_file_info* fi) {
  printf("======closing the file %s======\n", path);
  return fs_release(fs, path, fi);
}

static int do_utimens(const char* path, const struct timespec tv[2]) {
  printf("=======setting times======\n");
  return fs_utimens(fs, path, tv);
}

static int do_truncate(const char* path, off_t size) {
  printf("=======truncating file %s======\n", path);
  return fs_truncate(fs, path, size);
}

static int do_unlink(const char* path) {
  printf("======removing file %s======\n", path);
  return fs_unlink(fs, path);
}

static int do_rmdir(const char* path) {
  printf("======removing directory %s======\n", path);
  return fs_rmdir(fs, path);
}

static int do_link(const char* oldpath, const char* newpath) {
  printf("======creating hard link=======\n");
  return fs_link(fs, oldpath, newpath);
}

static int do_symbolic_link(const char* target, const char* linkpath) {
  printf("======creating symbolic link======\n");
  return fs_symbolic_link(fs, target, linkpath);
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .mknod = do_mknod,
    .mkdir = do_mkdir,
    .read = do_read,
    .write = do_write,
    .readdir = do_readdir,
    .create = do_create,
    .open = do_open,
    .release = do_release,
    .utimens = do_utimens,
    .truncate = do_truncate,
    .unlink = do_unlink,
    .rmdir = do_rmdir,
    .link = do_link,
    .symbolic_link = do_symbolic_link,
};

int main(int argc, char* argv[]) {
  printf("starting the file system\n");

  fs = (filesystem_t*)malloc(sizeof(filesystem_t));

  assert(fs != NULL);

  fs_init(fs);

  return fuse_main(argc, argv, &operations, NULL);
}
