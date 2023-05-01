#include "node.h"

DirNode::DirNode() {}

DirNode::DirNode(std::string name, Node* parent) {
  name = name;
  parent = parent;
  type = NodeType::DIRNODE;

  DirNode dot(".", parent);
  DirNode dotdot("..", parent->parent);

  children.emplace_back(dot);
  children.emplace_back(dotdot);
}

bool DirNode::exist(std::string name) {
  for (auto child: this->children) {
    if (child->name == name) return true;
  }

  return false;
}

int DirNode::mkdir(std::string name) {
  if (exist(name)) {
    std::perror("node already exist");
    return -1;
  }

  children.emplace_back(new DirNode(name, this));

  return 0;
}

int DirNode::mknod(std::string name) {
  if (exist(name)) {
    std::perror("node already exist");
    return -1;
  }

  children.emplace_back(new FileNode(name, this));

  return 0;
}

std::vector<Node*> DirNode::lsdir() {
  return children;
}

FileNode::FileNode(std::string name, Node* parent) {
  name = name;
  parent = parent;
  type = NodeType::FILENODE;
}

void FileNode::write(std::string content) {
  content = content;
}

std::string FileNode::read() {
  return content;
}
