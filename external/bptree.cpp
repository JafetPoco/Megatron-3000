#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

template <typename T> class BPlusNode {
public:
  bool isLeaf;
  vector<T> keys;
  vector<BPlusNode<T> *> children;
  BPlusNode<T> *next;

  BPlusNode(bool leaf = false) : isLeaf(leaf), next(nullptr) {}
};

template <typename T> class BPlusTree {
private:
  BPlusNode<T> *root;
  int t;

public:
  BPlusTree(int t) : root(nullptr), t(t) {}

  bool search(const T &key) {
    if (!root)
      return false;
    BPlusNode<T> *node = root;
    while (!node->isLeaf) {
      int idx = upper_bound(node->keys.begin(), node->keys.end(), key) -
                node->keys.begin();
      node = node->children[idx];
    }
    return binary_search(node->keys.begin(), node->keys.end(), key);
  }

  void insert(const T &key) {
    if (!root) {
      root = new BPlusNode<T>(true);
      root->keys.push_back(key);
      return;
    }
    T newKey;
    BPlusNode<T> *newChild = nullptr;
    if (insertInternal(root, key, newKey, newChild)) {
      BPlusNode<T> *newRoot = new BPlusNode<T>(false);
      newRoot->keys.push_back(newKey);
      newRoot->children.push_back(root);
      newRoot->children.push_back(newChild);
      root = newRoot;
    }
  }

  void remove(const T &key) {
    if (!root)
      return;
    removeInternal(root, key);
    if (!root->isLeaf && root->children.size() == 1) {
      BPlusNode<T> *tmp = root;
      root = root->children[0];
      delete tmp;
    }
    if (root->isLeaf && root->keys.empty()) {
      delete root;
      root = nullptr;
    }
  }

  void printTree() {
    if (!root) {
      cout << "<empty>\n";
      return;
    }
    vector<BPlusNode<T> *> level = {root};
    cout << "Tree Levels:\n";
    while (!level.empty()) {
      vector<BPlusNode<T> *> next;
      for (auto node : level) {
        cout << "[";
        for (auto &k : node->keys)
          cout << k << " ";
        cout << "] ";
        if (!node->isLeaf)
          next.insert(next.end(), node->children.begin(), node->children.end());
      }
      cout << "\n";
      level = move(next);
    }

    cout << "Leaf Level: ";
    BPlusNode<T> *leaf = root;
    while (leaf && !leaf->isLeaf)
      leaf = leaf->children[0];
    while (leaf) {
      cout << "[";
      for (auto &k : leaf->keys)
        cout << k << " ";
      cout << "] -> ";
      leaf = leaf->next;
    }
    cout << "nullptr\n\n";
  }

private:
  int maxLeafKeys() const { return 2 * t-1; }
  int minLeafKeys() const { return t; }
  int maxInternalKeys() const { return 2 * t-1; }
  int minInternalKeys() const { return t; }

  bool insertInternal(BPlusNode<T> *node, const T &key, T &upKey,
                      BPlusNode<T> *&newChild) {
    if (node->isLeaf) {
      auto it = lower_bound(node->keys.begin(), node->keys.end(), key);
      node->keys.insert(it, key);
      if ((int)node->keys.size() > maxLeafKeys()) {
        splitLeaf(node, upKey, newChild);
        return true;
      }
      return false;
    }
    int idx = upper_bound(node->keys.begin(), node->keys.end(), key) -
              node->keys.begin();
    T childUp;
    BPlusNode<T> *childNew = nullptr;
    if (insertInternal(node->children[idx], key, childUp, childNew)) {
      node->keys.insert(node->keys.begin() + idx, childUp);
      node->children.insert(node->children.begin() + idx + 1, childNew);
      if ((int)node->keys.size() > maxInternalKeys()) {
        splitInternal(node, upKey, newChild);
        return true;
      }
    }
    return false;
  }

  void splitLeaf(BPlusNode<T> *node, T &upKey, BPlusNode<T> *&newLeaf) {
    newLeaf = new BPlusNode<T>(true);
    int mid = node->keys.size() / 2;
    newLeaf->keys.assign(node->keys.begin() + mid, node->keys.end());
    node->keys.resize(mid);
    newLeaf->next = node->next;
    node->next = newLeaf;
    upKey = newLeaf->keys.front();
  }

  void splitInternal(BPlusNode<T> *node, T &upKey, BPlusNode<T> *&newNode) {
    newNode = new BPlusNode<T>(false);
    int mid = node->keys.size() / 2;
    upKey = node->keys[mid];
    newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    newNode->children.assign(node->children.begin() + mid + 1,
                             node->children.end());
    node->keys.resize(mid);
    node->children.resize(mid + 1);
  }

  bool removeInternal(BPlusNode<T> *node, const T &key) {
    if (node->isLeaf) {
      auto it = find(node->keys.begin(), node->keys.end(), key);
      if (it == node->keys.end())
        return false;
      node->keys.erase(it);
      return node->keys.size() < (size_t)minLeafKeys();
    }

    int idx = upper_bound(node->keys.begin(), node->keys.end(), key) -
              node->keys.begin();
    bool underflow = removeInternal(node->children[idx], key);
    if (!underflow)
      return false;

    BPlusNode<T> *child = node->children[idx];
    BPlusNode<T> *left = (idx > 0 ? node->children[idx - 1] : nullptr);
    BPlusNode<T> *right =
        (idx + 1 < node->children.size() ? node->children[idx + 1] : nullptr);

    if (left && left->keys.size() > minInternalKeys()) {
      child->keys.insert(child->keys.begin(), node->keys[idx - 1]);
      node->keys[idx - 1] = left->keys.back();
      left->keys.pop_back();
      if (!left->isLeaf) {
        child->children.insert(child->children.begin(), left->children.back());
        left->children.pop_back();
      }
    } else if (right && right->keys.size() > minInternalKeys()) {
      child->keys.push_back(node->keys[idx]);
      node->keys[idx] = right->keys.front();
      right->keys.erase(right->keys.begin());
      if (!right->isLeaf) {
        child->children.push_back(right->children.front());
        right->children.erase(right->children.begin());
      }
    } else if (left) {
      mergeNodes(left, child, node->keys[idx - 1]);
      node->children.erase(node->children.begin() + idx);
      node->keys.erase(node->keys.begin() + idx - 1);
      delete child;
    } else if (right) {
      mergeNodes(child, right, node->keys[idx]);
      node->children.erase(node->children.begin() + idx + 1);
      node->keys.erase(node->keys.begin() + idx);
      delete right;
    }

    return node->keys.size() < (size_t)minInternalKeys();
  }

  void mergeNodes(BPlusNode<T> *left, BPlusNode<T> *right, const T &sep) {
    if (!left->isLeaf) {
      left->keys.push_back(sep);
      left->keys.insert(left->keys.end(), right->keys.begin(),
                        right->keys.end());
      left->children.insert(left->children.end(), right->children.begin(),
                            right->children.end());
    } else {
      left->keys.insert(left->keys.end(), right->keys.begin(),
                        right->keys.end());
      left->next = right->next;
    }
  }
};

int main() {
  int t;
  string command;
  int key;

  cout << "B+ Tree\n";
  cout << "Minimum degree (t): ";
  if (!(cin >> t))
    return 1;

  BPlusTree<int> tree(t);

  cout << "\nCommands:\n";
  cout << "  insert <key>\n";
  cout << "  delete <key>\n";
  cout << "  search <key>\n";
  cout << "  display\n";
  cout << "  exit\n";

  string line;
  while (true) {
    cout << ">>> ";
    if (!(cin >> command))
      break;

    if (command == "insert") {
      if (cin >> key)
        tree.insert(key), cout << "Inserted " << key << ".";
      else
        break;
    } else if (command == "delete") {
      if (cin >> key)
        tree.remove(key), cout << "Deleted " << key << ".";
      else
        break;
    } else if (command == "search") {
      if (cin >> key) {
        bool found = tree.search(key);
        cout << (found ? "se encontro" : "no se encontro");
      } else
        break;
    } else if (command == "display") {
      tree.printTree();
    } else if (command == "exit") {
      break;
    } else {
      cout << "Unknown command. Try again.";
    }
    cout << "\n";
  }

  return 0;
}
