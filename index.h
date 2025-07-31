#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "bptree2.h"
#include "file.h"
#include "recordManager.h"
#include "schema.h"
#include <stdexcept>
#include <string>
#include <vector>

class IndexManager {
private:
  BPlusTree *tree = nullptr;
  std::string curTable;

  // Divide un string en trozos de tamaño <= chunkSize
  static std::vector<std::string> splitStringChunks(const std::string &s,
                                                    size_t chunkSize) {
    std::vector<std::string> out;
    for (size_t i = 0; i < s.size(); i += chunkSize) {
      out.push_back(s.substr(i, chunkSize));
    }
    return out;
  }

public:
  IndexManager() = default;

  ~IndexManager() { delete tree; }

  // Carga el índice de disco; si ya estaba cargada la misma tabla, no hace
  // nada.
  void loadIndex(const std::string &tableName) {
    cout<<"IM: Cargando indice "<<tableName<<'\n';
    if (tableName == curTable)
      return;
    delete tree;
    tree = new BPlusTree(16);
    curTable = tableName;

    File f(tableName + "tree");
    if (!f.isOpen())
      return;

    std::string serial;
    do {
      serial += f.accessBlock();
      cout<<"IM: "<<serial.size()<<' ';
    } while (f.nextBlock());
    f.close();

    if (!tree->readSerialized(serial)) {
      std::cerr << "[WARN] No se pudo deserializar índice de " << tableName
                << std::endl;
    }
  }

  // Crea el índice sobre la primera columna (campo 0) de la tabla.
  // Requiere que la tabla ya exista en disco y que su esquema esté cargado.
  void createIndex(const std::string &tableName, const Schema &schm) {
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

  // Escribe en disco el índice serializado en bloques
  void persist() {
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

  // Accede al árbol en memoria
  BPlusTree *getIndex() const { return tree; }
};

#endif // INDEX_MANAGER_H
