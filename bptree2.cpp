#include "bptree2.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>

bool BPlusTree::readSerialized(const std::string &serialized){
  istringstream ss(serialized);
  std::string val;
  while(ss >> val){
    insert(stoi(val));
  }
}

void BPlusTree::insert(int key) {
  if (!root) {
    root = new Node(true);
    root->keys.push_back(key);
    return;
  }

  Node* newChild = nullptr;
  int newKey = insertInternal(root, key, newChild);

  if (newChild) {
    Node* newRoot = new Node(false);
    newRoot->keys.push_back(newKey);
    newRoot->children.push_back(root);
    newRoot->children.push_back(newChild);
    root = newRoot;
  }
}

void BPlusTree::remove(int key) {
  removeInternal(root, key);
  if (root && !root->isLeaf && root->children.size() == 1) {
    Node* oldRoot = root;
    root = root->children[0];
    delete oldRoot;
  }
}

void BPlusTree::print() {
  cout << "\n----- B+ Tree -----\n";
  if (!root) {
    cout << "(empty)\n";
    return;
  }
  queue<Node*> q;
  q.push(root);
  while (!q.empty()) {
    int sz = q.size();
    for (int i = 0; i < sz; i++) {
      Node* node = q.front(); q.pop();
      cout << "[";
      for (int k : node->keys) cout << k << " ";
      cout << "] ";
      if (!node->isLeaf)
      for (Node* child : node->children) q.push(child);
    }
    cout << "\n";
  }
  cout << "-------------------\n";
}

int BPlusTree::insertInternal(Node* node, int key, Node*& newChild) {
  if (node->isLeaf) {
    node->keys.insert(upper_bound(node->keys.begin(), node->keys.end(), key), key);
    if (node->keys.size() >= order) {
      Node* sibling = new Node(true);
      int mid = node->keys.size() / 2;
      sibling->keys.assign(node->keys.begin() + mid, node->keys.end());
      node->keys.erase(node->keys.begin() + mid, node->keys.end());

      sibling->next = node->next;
      node->next = sibling;
      newChild = sibling;
      return sibling->keys[0];
    }
    return -1;
  } else {
    int idx = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
    Node* child = node->children[idx];
    Node* childNew = nullptr;
    int newKey = insertInternal(child, key, childNew);

    if (childNew) {
      node->keys.insert(node->keys.begin() + idx, newKey);
      node->children.insert(node->children.begin() + idx + 1, childNew);
      if (node->keys.size() >= order) {
        Node* sibling = new Node(false);
        int mid = node->keys.size() / 2;

        sibling->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        sibling->children.assign(node->children.begin() + mid + 1, node->children.end());

        int upKey = node->keys[mid];

        node->keys.erase(node->keys.begin() + mid, node->keys.end());
        node->children.erase(node->children.begin() + mid + 1, node->children.end());

        newChild = sibling;
        return upKey;
      }
    }
    return -1;
  }
}

void removeInternal(Node* node, int key) {
  if (node->isLeaf) {
    auto it = find(node->keys.begin(), node->keys.end(), key);
    if (it != node->keys.end()) node->keys.erase(it);
    return;
  }

  int idx = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
  if (idx > 0 && key < node->keys[idx - 1]) idx--;

  Node* child = node->children[idx];
  removeInternal(child, key);

  if (child->keys.size() < (order - 1) / 2) {
    Node* left = (idx > 0) ? node->children[idx - 1] : nullptr;
    Node* right = (idx + 1 < node->children.size()) ? node->children[idx + 1] : nullptr;

    if (left && left->keys.size() > (order - 1) / 2) {
      // rotate from left
      child->keys.insert(child->keys.begin(), node->keys[idx - 1]);
      node->keys[idx - 1] = left->keys.back();
      left->keys.pop_back();

      if (!left->isLeaf) {
        child->children.insert(child->children.begin(), left->children.back());
        left->children.pop_back();
      }
    } else if (right && right->keys.size() > (order - 1) / 2) {
      // rotate from right
      child->keys.push_back(node->keys[idx]);
      node->keys[idx] = right->keys.front();
      right->keys.erase(right->keys.begin());

      if (!right->isLeaf) {
        child->children.push_back(right->children.front());
        right->children.erase(right->children.begin());
      }
    } else {
      // merge
      if (left) {
        left->keys.push_back(node->keys[idx - 1]);
        left->keys.insert(left->keys.end(), child->keys.begin(), child->keys.end());
        if (!child->isLeaf)
          left->children.insert(left->children.end(), child->children.begin(), child->children.end());
        left->next = child->next;
        node->keys.erase(node->keys.begin() + idx - 1);
        node->children.erase(node->children.begin() + idx);
        delete child;
      } else if (right) {
        child->keys.push_back(node->keys[idx]);
        child->keys.insert(child->keys.end(), right->keys.begin(), right->keys.end());
        if (!right->isLeaf)
          child->children.insert(child->children.end(), right->children.begin(), right->children.end());
        child->next = right->next;
        node->keys.erase(node->keys.begin() + idx);
        node->children.erase(node->children.begin() + idx + 1);
        delete right;
      }
    }
  }
}

int main() {
  BPlusTree tree;

  string input = "100 200 300 50 420 500 250 260 10 20 210 92 93 94 96 97";

  for (int x : datos) {
    cout << "Insertar: " << x << "\n";
    tree.insert(x);
  }

  tree.print();

  // Prueba de eliminación
  cout << "\nEliminar: 210, 260, 50\n";
  tree.remove(210);
  tree.remove(260);
  tree.remove(50);
  tree.remove(200);
  tree.remove(250);

  tree.print();

  return 0;
}
