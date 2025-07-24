#ifndef BPTREE2_H
#define BPTREE2_H

#include <vector>
#include <iostream>

#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define RESET   "\033[0m"

using namespace std;

struct Value {
  int key;
  int position;

  bool operator<(const Value& other) const {
    return key < other.key;
  }

  bool operator==(const int otherKey) const {
    return key == otherKey;
  }
};

class BPlusTree {
 public:
  BPlusTree(int order) : order(order), root(nullptr) {}

  void insert(Value val);
  void remove(int key);
  void print();

  bool readSerialized(const std::string &serialized);
  std::string getSerialized() const;

 private:
  struct Node {
    bool isLeaf;
    vector<Value> keys;
    vector<Node*> children;
    Node* next;

    Node(bool leaf) : isLeaf(leaf), next(nullptr) {}
  };

  Node* root;
  int order;

  Value insertInternal(Node* node, Value val, Node*& newChild);
  void removeInternal(Node* node, int key);
};

#endif
