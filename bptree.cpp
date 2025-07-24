#include "bptree.h"
// BPlusTree.cpp #include "BPlusTree.h" #include <sstream> #include <queue>
// #include <algorithm>

const int MAX_DEGREE = 3;

BPlusTreeNode::BPlusTreeNode(bool leaf) : isLeaf(leaf), nextLeaf(nullptr) {}

void BPlusTree::insertInternal(int key, std::shared_ptr<BPlusTreeNode> child,
                               std::shared_ptr<BPlusTreeNode> node,
                               int &promotedKey,
                               std::shared_ptr<BPlusTreeNode> &newChild) {
  auto it = std::upper_bound(node->keys.begin(), node->keys.end(), key);
  int index = it - node->keys.begin();

  if (node->children[index]->isLeaf) {
    insertInLeaf(node->children[index], key, child->entries[0].pageId);
  } else {
    int dummy;
    std::shared_ptr<BPlusTreeNode> splitNode;
    insertInternal(key, child, node->children[index], dummy, splitNode);

    if (splitNode) {
      node->keys.insert(node->keys.begin() + index, dummy);
      node->children.insert(node->children.begin() + index + 1, splitNode);
    }
  }

  if (node->keys.size() >= MAX_DEGREE) {
    int mid = node->keys.size() / 2;
    promotedKey = node->keys[mid];
    newChild = std::make_shared<BPlusTreeNode>(false);
    newChild->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    newChild->children.assign(node->children.begin() + mid + 1,
                              node->children.end());
    node->keys.resize(mid);
    node->children.resize(mid + 1);
  }
}

void BPlusTree::insertInLeaf(std::shared_ptr<BPlusTreeNode> leaf, int key,
                             int pageId) {
  auto it = leaf->entries.begin();
  while (it != leaf->entries.end() && it->key < key)
    ++it;
  leaf->entries.insert(it, {key, pageId});
}

std::shared_ptr<BPlusTreeNode>
BPlusTree::buildTreeFromStream(std::istringstream &in) {
  std::string token;
  std::getline(in, token, '|');
  if (token.empty())
    return nullptr;

  std::istringstream part(token);
  std::string type;
  std::getline(part, type, ':');

  auto node = std::make_shared<BPlusTreeNode>(type == "L");

  std::string keysStr;
  std::getline(part, keysStr, ';');
  std::istringstream keysStream(keysStr);
  std::string key;
  while (std::getline(keysStream, key, ',')) {
    if (!key.empty())
      node->keys.push_back(std::stoi(key));
  }

  if (node->isLeaf) {
    std::string entriesStr;
    std::getline(part, entriesStr, '|');
    std::istringstream entriesStream(entriesStr);
    std::string entry;
    while (std::getline(entriesStream, entry, ',')) {
      if (!entry.empty()) {
        size_t eq = entry.find('=');
        int k = std::stoi(entry.substr(0, eq));
        int p = std::stoi(entry.substr(eq + 1));
        node->entries.push_back({k, p});
      }
    }
  } else {
    for (int i = 0; i <= node->keys.size(); ++i) {
      node->children.push_back(buildTreeFromStream(in));
    }
  }
  return node;
}

BPlusTree::BPlusTree() { root = std::make_shared<BPlusTreeNode>(true); }

BPlusTree::BPlusTree(const std::string &serialized) {
  std::istringstream in(serialized);
  root = buildTreeFromStream(in);
}

void BPlusTree::insert(int key, int pageId) {
  if (root->isLeaf) {
    insertInLeaf(root, key, pageId);
    if (root->entries.size() >= MAX_DEGREE) {
      int mid = MAX_DEGREE / 2;
      auto newLeaf = std::make_shared<BPlusTreeNode>(true);
      newLeaf->entries.assign(root->entries.begin() + mid, root->entries.end());
      root->entries.resize(mid);
      newLeaf->nextLeaf = root->nextLeaf;
      root->nextLeaf = newLeaf;

      auto newRoot = std::make_shared<BPlusTreeNode>(false);
      newRoot->keys.push_back(newLeaf->entries[0].key);
      newRoot->children.push_back(root);
      newRoot->children.push_back(newLeaf);
      root = newRoot;
    }
  } else {
    int promotedKey;
    std::shared_ptr<BPlusTreeNode> newChild;
    insertInternal(key, std::make_shared<BPlusTreeNode>(true), root,
                   promotedKey, newChild);
    if (newChild) {
      auto newRoot = std::make_shared<BPlusTreeNode>(false);
      newRoot->keys.push_back(promotedKey);
      newRoot->children.push_back(root);
      newRoot->children.push_back(newChild);
      root = newRoot;
    }
  }
}

int BPlusTree::search(int key) {
  auto node = root;
  while (!node->isLeaf) {
    int i = 0;
    while (i < node->keys.size() && key >= node->keys[i])
      ++i;
    node = node->children[i];
  }
  for (auto &entry : node->entries) {
    if (entry.key == key)
      return entry.pageId;
  }
  return -1;
}

std::string BPlusTree::serialize() {
  std::ostringstream out;
  std::queue<std::shared_ptr<BPlusTreeNode>> q;
  q.push(root);
  while (!q.empty()) {
    auto node = q.front();
    q.pop();
    out << (node->isLeaf ? "L" : "I") << ":";
    for (int k : node->keys)
      out << k << ",";
    out << ";";
    if (node->isLeaf) {
      for (auto &e : node->entries)
        out << e.key << "=" << e.pageId << ",";
    } else {
      for (auto &c : node->children)
        q.push(c);
    }
    out << "|";
  }
  return out.str();
}
