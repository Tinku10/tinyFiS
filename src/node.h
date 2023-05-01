#include <iostream>
#include <vector>

enum struct NodeType {
  FILENODE,
  DIRNODE
};

class Visitor;

class Node {
public:
  std::string name;
  Node* parent;
  NodeType type;
};

class DirNode: public Node {
public:
  std::vector<Node*> children;

  DirNode();
  DirNode(std::string name, Node* parent = nullptr);
  bool exist(std::string name);
  int mkdir(std::string name);
  /* int rmdir(std::string name); */
  int mknod(std::string name);
  /* int rmnod(std::string name); */
  std::vector<Node*> lsdir();
};

class FileNode: public Node {
public:
  std::string content;

  FileNode(std::string name, Node* parent = nullptr);
  void write(std::string content);
  std::string read();
};

class Visitor {
public:
  DirNode visit(DirNode& dir);
  FileNode visit(FileNode& file);
};
