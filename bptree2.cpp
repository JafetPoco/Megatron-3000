#include "bptree2.h"
#include <algorithm>
#include <queue>
#include <sstream>

using namespace std;

void BPlusTree::insert(Value val) {
  if (!root) {
    root = new Node(true);
    root->keys.push_back(val);
    return;
  }

  Node* newChild = nullptr;
  Value newKey = insertInternal(root, val, newChild);

  if (newChild) {
    Node* newRoot = new Node(false);
    newRoot->keys.push_back(newKey);
    newRoot->children.push_back(root);
    newRoot->children.push_back(newChild);
    root = newRoot;
  }
}

Value BPlusTree::insertInternal(Node* node, Value val, Node*& newChild) {
  if (node->isLeaf) {
    auto it = upper_bound(node->keys.begin(), node->keys.end(), val);
    node->keys.insert(it, val);

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
    return {-1, -1};
  } else {
    auto it = upper_bound(node->keys.begin(), node->keys.end(), val);
    int idx = it - node->keys.begin();
    Node* child = node->children[idx];

    Node* childNew = nullptr;
    Value newKey = insertInternal(child, val, childNew);

    if (childNew) {
      node->keys.insert(node->keys.begin() + idx, newKey);
      node->children.insert(node->children.begin() + idx + 1, childNew);

      if (node->keys.size() >= order) {
        Node* sibling = new Node(false);
        int mid = node->keys.size() / 2;

        sibling->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        sibling->children.assign(node->children.begin() + mid + 1, node->children.end());

        Value upKey = node->keys[mid];

        node->keys.erase(node->keys.begin() + mid, node->keys.end());
        node->children.erase(node->children.begin() + mid + 1, node->children.end());

        newChild = sibling;
        return upKey;
      }
    }
    return {-1, -1};
  }
}

void BPlusTree::remove(int key) {
  if (!root) return;

  removeInternal(root, key);

  if (!root->isLeaf && root->children.size() == 1) {
    Node* oldRoot = root;
    root = root->children[0];
    delete oldRoot;
  }
}

void BPlusTree::removeInternal(Node* node, int key) {
  if (node->isLeaf) {
    auto it = find_if(node->keys.begin(), node->keys.end(),
                      [&](const Value& v) { return v.key == key; });
    if (it != node->keys.end()) node->keys.erase(it);
    return;
  }

  auto it = upper_bound(node->keys.begin(), node->keys.end(), Value{key, 0});
  int idx = it - node->keys.begin();
  Node* child = node->children[idx];

  removeInternal(child, key);

  // Simplified rebalance logic
  if (child->keys.size() < (order - 1) / 2) {
    Node* left = (idx > 0) ? node->children[idx - 1] : nullptr;
    Node* right = (idx + 1 < node->children.size()) ? node->children[idx + 1] : nullptr;

    if (left && left->keys.size() > (order - 1) / 2) {
      child->keys.insert(child->keys.begin(), node->keys[idx - 1]);
      node->keys[idx - 1] = left->keys.back();
      left->keys.pop_back();
    } else if (right && right->keys.size() > (order - 1) / 2) {
      child->keys.push_back(node->keys[idx]);
      node->keys[idx] = right->keys.front();
      right->keys.erase(right->keys.begin());
    } else {
      if (left) {
        left->keys.push_back(node->keys[idx - 1]);
        left->keys.insert(left->keys.end(), child->keys.begin(), child->keys.end());
        left->next = child->next;
        node->keys.erase(node->keys.begin() + idx - 1);
        node->children.erase(node->children.begin() + idx);
        delete child;
      } else if (right) {
        child->keys.push_back(node->keys[idx]);
        child->keys.insert(child->keys.end(), right->keys.begin(), right->keys.end());
        child->next = right->next;
        node->keys.erase(node->keys.begin() + idx);
        node->children.erase(node->children.begin() + idx + 1);
        delete right;
      }
    }
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
      for (const auto& k : node->keys) cout << "(" <<YELLOW<<k.key <<RESET<<", " <<BLUE<<k.position<<RESET<<") ";
      cout << "] ";
      if (!node->isLeaf) {
        for (auto child : node->children) q.push(child);
      }
    }
    cout << "\n";
  }
  cout << "-------------------\n";
}

bool BPlusTree::readSerialized(const std::string &serialized){
  istringstream ss(serialized);
  string value;
  while(ss >> value){
    int key = stoi(value);
    ss >> value;
    insert({key, stoi(value)});
  }
  return true;
}

std::string BPlusTree::getSerialized() const {
  if (!root) {
    return "";
  }

  stringstream ss;

  // 1. Buscar la primera hoja
  Node* leaf = root;
  while (!leaf->isLeaf) {
    leaf = leaf->children[0];
  }

  // 2. Recorrer todas las hojas
  while (leaf) {
    for (const auto& k : leaf->keys) {
      ss << k.key << " " << k.position << " ";
    }
    leaf = leaf->next;
  }

  return ss.str();
}