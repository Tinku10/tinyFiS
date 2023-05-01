#include "fs.h"

int main(int argc, char* argv[]) {

  FileSystem fs;

  fuse_operations operations = {
    .getattr = fs.getattr,
    .readdir = fs.readdir,
    .read = fs.read,
    .write = fs.write,
    .mkdir = fs.mkdir,
    .mknod = fs.mknod
  };

  return fuse_main(argc, argv, &operations, NULL);
}

