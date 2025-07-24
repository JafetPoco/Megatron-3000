#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Node {
  bool isLeaf;
  vector<int> keys;
  vector<Node*> children;
  Node* next;

  Node(bool leaf = false) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
  Node* root;
  int order;

public:
  BPlusTree(int o) : root(nullptr), order(o) {}

  bool readSerialized(const std::string &serialized);
  std::string getSerialized() const;
  
  void insert(int key);
  void remove(int key);
  void print();

private:
  int insertInternal(Node* node, int key, Node*& newChild);
  void removeInternal(Node* node, int key);
};

#endif