#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "bptree2.h"
#include "file.h"
#include "schema.h"
#include <map>
#include <string>
#include <vector>
using namespace std;

class IndexManager {
private:
  BPlusTree *tree = nullptr;
  string curTable;

public:
  IndexManager() = default;
  void loadIndex(const string &tableName) {
    if (tableName == curTable) return;
    if (tree) {
      delete tree;
    }
    tree = new BPlusTree(16);
    File treeFile(tableName + "_btree");
    string content;
    do {
      content += treeFile.accessBlock();
    } while (treeFile.nextBlock());
    tree->readSerialized(content);
  }

  BPlusTree *getIndex() { return tree; }

  void createIndex(const string &tableName, const string &colName) {}
  void persist() {
    if (!tree)
      return;

    std::string data = tree->getSerialized();

    File file(curTable + "_btree", 'w');
    size_t cap = file.getCapacity();
    auto chunks = splitStringChunks(data, cap);

    for (size_t i = 0; i < chunks.size(); ++i) {
      std::string &block = file.accessBlock();
      block = chunks[i];

      if (i + 1 < chunks.size()) {
        if (!file.nextBlock()) {
          file.addBlock();
        }
      }
    }
    file.close();
  }
};

#endif
