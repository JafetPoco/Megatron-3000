#include "index.h"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

IndexManager::IndexManager() = default;

IndexManager::~IndexManager() {
  delete tree;
}

std::vector<std::string> IndexManager::splitStringChunks(const std::string &s, size_t chunkSize) {
  std::vector<std::string> out;
  for (size_t i = 0; i < s.size(); i += chunkSize) {
    out.push_back(s.substr(i, chunkSize));
  }
  return out;
}

void IndexManager::loadIndex(const std::string &tableName) {
  if (tableName == curTable)
    return;

  delete tree;
  tree = new BPlusTree(16);
  curTable = tableName;

  std::string filePath = tableName + "tree";
  File f(filePath);
  if (!f.isOpen()) {
    cerr << "[WARN] No se pudo abrir el archivo: " << filePath << '\n';
    return;
  }

  std::string serial;
  do {
    string tmp = f.accessBlock();
    tmp.erase(0, tmp.find_first_not_of(" \t\r\n"));
    tmp.erase(tmp.find_last_not_of(" \t\r\n") + 1);
    serial += tmp;
  } while (f.nextBlock());

  f.close();

  if (!tree->readSerialized(serial)) {
    cerr << "[WARN] No se pudo deserializar índice de " << tableName << '\n';
  }
}

void IndexManager::createIndex(const std::string &tableName, const Schema &schm) {
  delete tree;
  tree = new BPlusTree(16);
  curTable = tableName;

  File tableFile(tableName, 'r');
  if (!tableFile.isOpen())
    throw std::runtime_error("No se pudo abrir tabla " + tableName);

  RecordManagerFixed rm(tableName);
  int rowID = 0;

  do {
    int curpage = tableFile.getCurrent();
    std::string block = tableFile.accessBlock();
    if (block.empty())
      continue;

    auto rows = rm.parseFixedData(block, schm);
    for (auto &row : rows) {
      int key = std::stoi(row[0]);
      tree->insert({key, curpage});
      ++rowID;
    }
  } while (tableFile.nextBlock());

  tableFile.close();
  persist();
}

void IndexManager::persist() {
  if (!tree || curTable.empty())
    return;

  std::string serial = tree->getSerialized();
  File f(curTable + "tree", 'w');

  size_t cap = f.getCapacity();
  auto chunks = splitStringChunks(serial, cap);

  for (size_t i = 0; i < chunks.size(); ++i) {
    std::string &blk = f.accessBlock();
    blk = chunks[i];

    if (i + 1 < chunks.size()) {
      if (!f.nextBlock())
        f.addBlock();
    }
  }
  f.close();
}

BPlusTree *IndexManager::getIndex() const {
  return tree;
}
