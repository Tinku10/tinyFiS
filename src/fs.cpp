#include "fs.h"

FileSystem::FileSystem() {
  root = new DirNode("/");
  curr_dir = root;
  max_depth = 10;
}

int FileSystem::enter_dir(DirNode dir) {
  if (!curr_dir->exist(dir.name)) {
    return -1;
  }

  curr_dir = &dir;

  return 0;
}

int FileSystem::exit_dir() {
  if (curr_dir->parent == nullptr) {
    perror("cannot exit root");
    return -1;
  }

  curr_dir = static_cast<DirNode *>(curr_dir->parent);

  return 0;
}

Node FileSystem::nth_node(std::string path, int n) {
  // path always starts from root of the file system
  Node *curr = root;
  // start at 1 to avoid special case for '/' root
  int start = 1, end = path.length();
  // remove the last trailing '/'
  if (path.back() == '/')
    end--;

  int node_count = std::count(path.begin() + start, path.end() - n + end, '/');

  for (int i = start; i < end; i++) {
    int next = path.find('/');

    // base node
    if (next == std::string::npos || node_count == 0) {
      return *curr;
    }

    DirNode *dir = static_cast<DirNode *>(curr);

    for (auto child : dir->children) {
      if (child->name == path.substr(i, next - i)) {
        if (child->type == NodeType::DIRNODE)
          curr = child;
      }
    }

    node_count--;
    i = next;
  }
}

int FileSystem::getattr(const char *path, struct stat *st) {
  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  Node base = nth_node(path);

  if (base.type == NodeType::DIRNODE) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if (base.type == NodeType::FILENODE) {
    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = 1024;
  } else {
    return -ENOENT;
  }

  return 0;
}

int FileSystem::readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {

  Node base = nth_node(path);

  DirNode *dir = static_cast<DirNode *>(&base);

  for (auto child : dir->children) {
    filler(buffer, child->name, NULL, 0);
  }

  return 0;
}

int FileSystem::read(const char *path, char *buffer, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
  Node base = nth_node(path);

  FileNode *file = static_cast<FileNode *>(&base);

  int n = file->content.length();
  std::string content = file->content.substr(offset, n - offset);

  memcpy(buffer, content.c_str(), size);

  return n - offset;
}

int FileSystem::mkdir(const char *path, mode_t mode) {
  Node base = nth_node(path, 1);

  DirNode *dir = static_cast<DirNode *>(&base);

  DirNode *newdir = new DirNode(dir->name, dir);

  dir->children.emplace_back(newdir);

  return 0;
}

int FileSystem::mknod(const char *path, mode_t mode, dev_t dev) {
  Node base = nth_node(path, 1);

  DirNode *dir = static_cast<DirNode *>(&base);

  FileNode *newfile = new FileNode(dir->name, dir);

  dir->children.emplace_back(newfile);

  return 0;
}

int FileSystem::write(const char *path, const char *buffer, size_t size,
                      off_t offset, struct fuse_file_info *info) {
  Node base = nth_node(path);

  FileNode *newfile = static_cast<FileNode *>(&base);
  newfile->write(buffer);

  return size;
}
