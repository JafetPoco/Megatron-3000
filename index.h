#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "bptree2.h"
#include "file.h"
#include "recordManager.h"
#include "schema.h"
#include <string>
#include <vector>

class IndexManager {
private:
  BPlusTree *tree = nullptr;
  std::string curTable;

  static std::vector<std::string> splitStringChunks(const std::string &s,
                                                    size_t chunkSize);

public:
  IndexManager();
  ~IndexManager();

  void loadIndex(const std::string &tableName);
  void createIndex(const std::string &tableName, const Schema &schm);
  void persist();
  BPlusTree *getIndex() const;
};

#endif // INDEX_MANAGER_H
