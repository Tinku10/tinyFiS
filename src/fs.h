#include "node.h"
#include <fuse.h>

class FileSystem {
public:
  DirNode* root;
  int max_depth;
  DirNode* curr_dir;

  FileSystem();

  int enter_dir(DirNode dir);
  int exit_dir();
  // return the nth node from the end
  Node nth_node(std::string path, int n = 0);

  int getattr(const char* path, struct stat* st);
  int readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
  int read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* fi);
  int mkdir(const char* path, mode_t mode);
  int mknod(const char* path, mode_t mode, dev_t dev);
  int write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info);
};


