// B+ Tree of order 3 (max 2 keys per node)
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int ORDER = 3; // Max keys per node = ORDER - 1

struct BPlusNode {
    bool isLeaf;
    vector<int> keys;
    vector<BPlusNode*> children;
    BPlusNode* next;

    BPlusNode(bool leaf = false) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
    BPlusNode* root;

public:
    BPlusTree() : root(nullptr) {}

    void insert(int key) {
        if (!root) {
            root = new BPlusNode(true);
            root->keys.push_back(key);
            return;
        }

        BPlusNode* newChild = nullptr;
        int newKey = -1;
        bool split = insertInternal(root, key, newKey, newChild);

        if (split) {
            BPlusNode* newRoot = new BPlusNode(false);
            newRoot->keys.push_back(newKey);
            newRoot->children.push_back(root);
            newRoot->children.push_back(newChild);
            root = newRoot;
        }
    }

    void printTree() {
        vector<BPlusNode*> level;
        level.push_back(root);
        cout << "Tree Levels:\n";
        while (!level.empty()) {
            vector<BPlusNode*> nextLevel;
            for (BPlusNode* node : level) {
                cout << "[";
                for (int k : node->keys)
                    cout << k << " ";
                cout << "] ";
                if (!node->isLeaf)
                    nextLevel.insert(nextLevel.end(), node->children.begin(), node->children.end());
            }
            cout << "\n";
            level = nextLevel;
        }

        // Print leaf links
        cout << "Leaf Level: ";
        BPlusNode* leaf = root;
        while (!leaf->isLeaf)
            leaf = leaf->children[0];
        while (leaf) {
            cout << "[";
            for (int k : leaf->keys) cout << k << " ";
            cout << "] -> ";
            leaf = leaf->next;
        }
        cout << "nullptr\n\n";
    }

private:
    bool insertInternal(BPlusNode* node, int key, int& newKey, BPlusNode*& newChild) {
        if (node->isLeaf) {
            node->keys.insert(lower_bound(node->keys.begin(), node->keys.end(), key), key);
            if (node->keys.size() < ORDER)
                return false;
            splitLeaf(node, newKey, newChild);
            return true;
        }

        int idx = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
        bool split = insertInternal(node->children[idx], key, newKey, newChild);
        if (!split) return false;

        node->keys.insert(node->keys.begin() + idx, newKey);
        node->children.insert(node->children.begin() + idx + 1, newChild);

        if (node->keys.size() < ORDER)
            return false;

        splitInternal(node, newKey, newChild);
        return true;
    }


void splitLeaf(BPlusNode* node, int& newKey, BPlusNode*& newLeaf) {
    newLeaf = new BPlusNode(true);
    int mid = node->keys.size() / 2;
    newLeaf->keys.assign(node->keys.begin() + mid, node->keys.end());
    node->keys.resize(mid);
    newLeaf->next = node->next;
    node->next = newLeaf;
    newKey = newLeaf->keys[0]; // Correcta: se copia la primera clave del nuevo nodo
}


    void splitInternal(BPlusNode* node, int& newKey, BPlusNode*& newNode) {
        newNode = new BPlusNode(false);
        int mid = ORDER / 2;
        newKey = node->keys[mid];

        newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        newNode->children.assign(node->children.begin() + mid + 1, node->children.end());

        node->keys.resize(mid);
        node->children.resize(mid + 1);
    }
};

int main() {
    BPlusTree tree;
    vector<int> values = {10, 15, 35, 5, 20, 17, 62};
    for (int v : values) {
        cout << "Inserting: " << v << "\n";
        tree.insert(v);
        tree.printTree();
    }
    return 0;
}

