#include <algorithm>
#include <iostream>
#include <string> // Added for string
#include <vector>

using namespace std;

template <typename T> class BPlusNode {
public:
  bool isLeaf;
  vector<T> keys;
  vector<BPlusNode<T> *> children;
  BPlusNode<T> *next; // Pointer to the next leaf node

  BPlusNode(bool leaf = false) : isLeaf(leaf), next(nullptr) {}

  // Destructor to free memory
  ~BPlusNode() {
    if (!isLeaf) {
      for (BPlusNode<T> *child : children) {
        delete child;
      }
    }
  }
};

template <typename T> class BPlusTree {
private:
  BPlusNode<T> *root;
  int t; // Minimum degree

public:
  BPlusTree(int t) : root(nullptr), t(t) {}

  // Destructor for the tree
  ~BPlusTree() {
    delete root; // Recursively deletes all nodes
  }

  // Returns true if key is found, false otherwise
  bool search(const T &key) {
    if (!root)
      return false;
    BPlusNode<T> *node = root;
    while (!node->isLeaf) {
      // Find the child pointer to follow
      int idx = upper_bound(node->keys.begin(), node->keys.end(), key) -
                node->keys.begin();
      node = node->children[idx];
    }
    // Search in the leaf node
    return binary_search(node->keys.begin(), node->keys.end(), key);
  }

  // Inserts a key into the B+ tree
  void insert(const T &key) {
    if (!root) {
      root = new BPlusNode<T>(true); // Create a new leaf root
      root->keys.push_back(key);
      return;
    }

    T newKey;                         // Key to be promoted to parent
    BPlusNode<T> *newChild = nullptr; // New child created by split

    // If root splits, a new root is created
    if (insertInternal(root, key, newKey, newChild)) {
      BPlusNode<T> *newRoot = new BPlusNode<T>(false);
      newRoot->keys.push_back(newKey);
      newRoot->children.push_back(root);
      newRoot->children.push_back(newChild);
      root = newRoot;
    }
  }

  // Removes a key from the B+ tree
  void remove(const T &key) {
    if (!root)
      return;

    // removeInternal returns true if the child underflowed
    bool underflow = removeInternal(root, key);

    // Handle root cases after removal
    if (underflow) {                             // If root itself underflowed
      if (!root->isLeaf && root->keys.empty()) { // Internal root and empty
        BPlusNode<T> *tmp = root;
        root = root->children.front(); // New root is its only child
        tmp->children.clear(); // Prevent child deletion by tmp destructor
        delete tmp;
      } else if (root->isLeaf && root->keys.empty()) { // Leaf root and empty
        delete root;
        root = nullptr;
      }
    }
  }

  // Prints the tree level by level and the leaf linked list
  void printTree() {
    if (!root) {
      cout << "<empty>\n";
      return;
    }
    vector<BPlusNode<T> *> level_nodes;
    level_nodes.push_back(root);

    cout << "Tree Levels:\n";
    while (!level_nodes.empty()) {
      vector<BPlusNode<T> *> next_level_nodes;
      for (auto node : level_nodes) {
        cout << "[";
        for (size_t i = 0; i < node->keys.size(); ++i) {
          cout << node->keys[i] << (i == node->keys.size() - 1 ? "" : " ");
        }
        cout << "] ";
        if (!node->isLeaf) {
          for (BPlusNode<T> *child : node->children) {
            next_level_nodes.push_back(child);
          }
        }
      }
      cout << "\n";
      level_nodes = next_level_nodes;
    }

    cout << "Leaf Level: ";
    BPlusNode<T> *leaf = root;
    while (leaf && !leaf->isLeaf) {
      leaf = leaf->children[0]; // Traverse to the leftmost leaf
    }
    while (leaf) {
      cout << "[";
      for (size_t i = 0; i < leaf->keys.size(); ++i) {
        cout << leaf->keys[i] << (i == leaf->keys.size() - 1 ? "" : " ");
      }
      cout << "] -> ";
      leaf = leaf->next;
    }
    cout << "nullptr\n\n";
  }

private:
  // Max keys allowed in a leaf or internal node
  int maxKeys() const { return 2 * t - 1; }
  // Min keys allowed in a leaf or internal node (non-root)
  int minKeys() const { return t - 1; } // Standard B+ tree minimum

  // Helper for insertion
  bool insertInternal(BPlusNode<T> *node, const T &key, T &upKey,
                      BPlusNode<T> *&newChild) {
    if (node->isLeaf) {
      auto it = lower_bound(node->keys.begin(), node->keys.end(), key);
      node->keys.insert(it, key);
      if ((int)node->keys.size() > maxKeys()) { // Use single maxKeys()
        splitLeaf(node, upKey, newChild);
        return true;
      }
      return false;
    }
    // Recurse for internal nodes
    int idx = upper_bound(node->keys.begin(), node->keys.end(), key) -
              node->keys.begin();
    T childUp;
    BPlusNode<T> *childNew = nullptr;
    if (insertInternal(node->children[idx], key, childUp, childNew)) {
      // Child split, so insert childUp key and newChild pointer
      node->keys.insert(node->keys.begin() + idx, childUp);
      node->children.insert(node->children.begin() + idx + 1, childNew);
      if ((int)node->keys.size() > maxKeys()) { // Use single maxKeys()
        splitInternal(node, upKey, newChild);
        return true;
      }
    }
    return false;
  }

  // Splits a full leaf node
  void splitLeaf(BPlusNode<T> *node, T &upKey, BPlusNode<T> *&newLeaf) {
    newLeaf = new BPlusNode<T>(true);
    int mid = node->keys.size() / 2; // Middle index for splitting keys
    newLeaf->keys.assign(node->keys.begin() + mid, node->keys.end());
    node->keys.resize(mid);
    newLeaf->next = node->next; // Link new leaf in the list
    node->next = newLeaf;
    upKey = newLeaf->keys
                .front(); // Key to be promoted is the first key of the new leaf
  }

  // Splits a full internal node
  void splitInternal(BPlusNode<T> *node, T &upKey, BPlusNode<T> *&newNode) {
    newNode = new BPlusNode<T>(false);
    int mid = node->keys.size() / 2; // Mid index for keys
    upKey = node->keys[mid]; // Key at mid is promoted (and removed from node)
    // Copy keys from mid+1 to end to new node
    newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    // Copy children from mid+1 to end to new node
    newNode->children.assign(node->children.begin() + mid + 1,
                             node->children.end());
    node->keys.resize(mid);         // Resize original node's keys
    node->children.resize(mid + 1); // Resize original node's children
  }

  // Helper for removal
  // Returns true if the current node underflowed after removal
  bool removeInternal(BPlusNode<T> *node, const T &key) {
    int key_idx_in_node = -1;     // Index of key in current node if found (for
                                  // internal nodes, to delete)
    int child_idx_to_follow = -1; // Index of child to follow

    if (node->isLeaf) {
      auto it = find(node->keys.begin(), node->keys.end(), key);
      if (it == node->keys.end()) {
        return false; // Key not found in this leaf
      }
      node->keys.erase(it);
      // Return true if node is now under minimum (trigger rebalance)
      // Adjusted: Trigger underflow if keys < minKeys().
      // For t=2, minKeys() is 1. If keys.size() is 0, it's underflow.
      // However, the trace implies 1 key is also cause for rebalance (like
      // delete 25). Let's make it trigger if less than 't' keys for now, to try
      // to match the trace. The trace's behavior for 1 key being an underflow
      // is an implementation choice.
      return node->keys.size() <
             (size_t)minKeys(); // This is standard: minKeys=1, so underflow if
                                // 0 keys.
    }

    // Internal node: find child to descend into
    child_idx_to_follow =
        upper_bound(node->keys.begin(), node->keys.end(), key) -
        node->keys.begin();

    // Check if the key to be deleted exists in the current internal node.
    // This only applies if we are following the path to a leaf where the key
    // is the separator for two children, and one of those children's keys
    // (the one we delete) happens to be that separator.
    // In a B+ tree, internal node keys are copies. If a key is deleted from
    // leaf, and it was a separator, it might need to be replaced in the
    // internal node.
    if (child_idx_to_follow < node->keys.size() &&
        node->keys[child_idx_to_follow] == key) {
      key_idx_in_node = child_idx_to_follow;
    } else if (child_idx_to_follow > 0 &&
               node->keys[child_idx_to_follow - 1] == key) {
      // This case covers if the key is the separator for the current path
      // and is the largest key in the left child or smallest key in the right
      // child. This can be complex to manage directly. B+ trees usually update
      // keys only when a leaf split happens or a merge removes a separator. For
      // simplicity, let's assume `upper_bound` guides us correctly and we only
      // delete internal keys if they come "down" during a merge.
    }

    bool child_underflow =
        removeInternal(node->children[child_idx_to_follow], key);

    if (!child_underflow) {
      return false; // No underflow from child, nothing to do
    }

    // Child underflowed, rebalance current node
    BPlusNode<T> *child = node->children[child_idx_to_follow];
    BPlusNode<T> *left_sibling = (child_idx_to_follow > 0)
                                     ? node->children[child_idx_to_follow - 1]
                                     : nullptr;
    BPlusNode<T> *right_sibling =
        (child_idx_to_follow + 1 < node->children.size())
            ? node->children[child_idx_to_follow + 1]
            : nullptr;

    // Case 1: Try to borrow from left sibling
    // This needs careful handling for leaf vs internal borrowing
    if (left_sibling && left_sibling->keys.size() > minKeys()) {
      // Borrow from left
      if (child->isLeaf) {
        // Leaf-specific borrow: Rotate key from left sibling to child, update
        // parent
        T borrowed_key = left_sibling->keys.back();
        left_sibling->keys.pop_back();
        child->keys.insert(child->keys.begin(), borrowed_key);
        node->keys[child_idx_to_follow - 1] =
            borrowed_key; // Update parent separator
      } else {
        // Internal-specific borrow: Parent's key moves down, sibling's key
        // moves up
        T parent_key = node->keys[child_idx_to_follow - 1];
        T sibling_key = left_sibling->keys.back();
        BPlusNode<T> *sibling_child = left_sibling->children.back();

        left_sibling->keys.pop_back();
        left_sibling->children.pop_back();

        child->keys.insert(child->keys.begin(), parent_key);
        child->children.insert(child->children.begin(), sibling_child);

        node->keys[child_idx_to_follow - 1] = sibling_key;
      }
      return false; // Rebalanced, no underflow
    }
    // Case 2: Try to borrow from right sibling
    else if (right_sibling && right_sibling->keys.size() > minKeys()) {
      // Borrow from right
      if (child->isLeaf) {
        // Leaf-specific borrow: Rotate key from right sibling to child, update
        // parent
        T borrowed_key = right_sibling->keys.front();
        right_sibling->keys.erase(right_sibling->keys.begin());
        child->keys.push_back(borrowed_key);
        node->keys[child_idx_to_follow] =
            right_sibling->keys.front(); // Update parent separator
      } else {
        // Internal-specific borrow: Parent's key moves down, sibling's key
        // moves up
        T parent_key = node->keys[child_idx_to_follow];
        T sibling_key = right_sibling->keys.front();
        BPlusNode<T> *sibling_child = right_sibling->children.front();

        right_sibling->keys.erase(right_sibling->keys.begin());
        right_sibling->children.erase(right_sibling->children.begin());

        child->keys.push_back(parent_key);
        child->children.push_back(sibling_child);

        node->keys[child_idx_to_follow] = sibling_key;
      }
      return false; // Rebalanced, no underflow
    }
    // Case 3: Cannot borrow, must merge
    else {
      // Merge with a sibling
      if (left_sibling) { // Merge with left sibling
        mergeNodes(left_sibling, child, node->keys[child_idx_to_follow - 1]);
        node->children.erase(node->children.begin() + child_idx_to_follow);
        node->keys.erase(node->keys.begin() + child_idx_to_follow - 1);
        // Adjust leaf linking if merging leaves
        if (left_sibling->isLeaf) {
          left_sibling->next = child->next;
        }
        delete child;
      } else { // Merge with right sibling (left sibling doesn't exist)
        mergeNodes(child, right_sibling, node->keys[child_idx_to_follow]);
        node->children.erase(node->children.begin() + child_idx_to_follow + 1);
        node->keys.erase(node->keys.begin() + child_idx_to_follow);
        // Adjust leaf linking if merging leaves
        if (child->isLeaf) {
          child->next = right_sibling->next;
        }
        delete right_sibling;
      }
      // Return true if current node (parent) now underflowed
      return node->keys.size() < (size_t)minKeys();
    }
  }

  // Merges right node into left node
  void mergeNodes(BPlusNode<T> *left, BPlusNode<T> *right, const T &sep) {
    if (!left->isLeaf) {
      // Internal node merge: bring down separator, then append keys/children
      left->keys.push_back(sep);
      left->keys.insert(left->keys.end(), right->keys.begin(),
                        right->keys.end());
      left->children.insert(left->children.end(), right->children.begin(),
                            right->children.end());
    } else {
      // Leaf node merge: just append keys
      left->keys.insert(left->keys.end(), right->keys.begin(),
                        right->keys.end());
      left->next = right->next; // Update leaf next pointer
    }
  }
};

int main() {
  int t_val; // Renamed 't' to 't_val' to avoid shadowing member variable 't'
  string command;
  int key;

  cout << "B+ Tree\n";
  cout << "Minimum degree (t): ";
  if (!(cin >> t_val))
    return 1;

  BPlusTree<int> tree(t_val); // Use t_val here

  cout << "\nCommands:\n";
  cout << "  insert <key>\n";
  cout << "  delete <key>\n";
  cout << "  search <key>\n";
  cout << "  display\n";
  cout << "  exit\n";

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
}
