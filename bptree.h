// BPlusTree.h
#pragma once
#include <memory>
#include <string>
#include <vector>

struct Entry {
  int key;
  int pageId;
};

class BPlusTreeNode {
public:
  bool isLeaf;
  std::vector<int> keys;
  std::vector<std::shared_ptr<BPlusTreeNode>> children;
  std::vector<Entry> entries;
  std::shared_ptr<BPlusTreeNode> nextLeaf;

  BPlusTreeNode(bool leaf = false);
};

class BPlusTree {
private:
  std::shared_ptr<BPlusTreeNode> root;
  void insertInternal(int key, std::shared_ptr<BPlusTreeNode> child,
                      std::shared_ptr<BPlusTreeNode> node, int &promotedKey,
                      std::shared_ptr<BPlusTreeNode> &newChild);
  void insertInLeaf(std::shared_ptr<BPlusTreeNode> leaf, int key, int pageId);
  std::shared_ptr<BPlusTreeNode> buildTreeFromStream(std::istringstream &in);

public:
  BPlusTree();
  BPlusTree(const std::string &serialized);
  void insert(int key, int pageId);
  int search(int key);
  std::string serialize();
};

