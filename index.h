#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <string>
#include <vector>
#include <map>
using namespace std;

class Index {
public:
  Index() = default;
  Index(const string& tableName, const string& colName) {}
  vector<int> search(const string& op, const string& value) {
    return {}; // Simula offsets de coincidencias
  }
};

class IndexManager {
private:
  map<string, Index> indices;

public:
  IndexManager() = default;
  Index& getIndex(const string& tableName, const string& colName) {
    string key = tableName + "_" + colName;
    if (!indices.count(key))
      indices[key] = Index(tableName, colName);
    return indices[key];
  }

  void loadAllIndices(const string& tableName) {
    // Simula cargar todos los índices de esa tabla
  }

  void createIndex(const string& tableName, const string& colName) {
    // Simula crear índice para la columna
  }
};

#endif

