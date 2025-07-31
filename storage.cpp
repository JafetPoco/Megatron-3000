#include "storage.h"
#include "bptree2.h"
#include "csv.h"
#include "file.h"
#include "globals.h"
#include "recordManager.h"
#include "schema.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

storageManager::storageManager() {
  this->tableName = "sens";
  im.loadIndex("sens");
}

storageManager::~storageManager() {
  im.persist();
}
inline std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  size_t end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

bool compare(const string &left, const string &op, const string &right,
             FieldType type) {
  if (type == FieldType::INT) {
    int l = stoi(left), r = stoi(right);
    if (op == "=")
      return l == r;
    if (op == "!=")
      return l != r;
    if (op == "<")
      return l < r;
    if (op == "<=")
      return l <= r;
    if (op == ">")
      return l > r;
    if (op == ">=")
      return l >= r;
  } else if (type == FieldType::DOUBLE) {
    double l = stod(left), r = stod(right);
    if (op == "=")
      return l == r;
    if (op == "!=")
      return l != r;
    if (op == "<")
      return l < r;
    if (op == "<=")
      return l <= r;
    if (op == ">")
      return l > r;
    if (op == ">=")
      return l >= r;
  } else {
    // cout<<left<<" "<<right<<endl;
    if (op == "=")
      return left == right;
    if (op == "!=")
      return left != right;
    if (op == "<")
      return left < right;
    if (op == "<=")
      return left <= right;
    if (op == ">")
      return left > right;
    if (op == ">=")
      return left >= right;
  }
  return false;
}

bool storageManager::uploadCSV(string csvfile, string tableName) {
  try {
    cout << "Subiendo csv " << csvfile << " con nombre " << tableName << '\n';
    schemas->uploadCsv(csvfile, tableName);

    // Escribimos los registros en disco
    RecordManagerFixed rm(tableName);
    CSVProcessor csv(csvfile);
    csv.process();
    vector<Record> test = csv.getData();

    schm = schemas->getSchema(tableName);
    auto formatted = rm.formatRows(test, schm);
    rm.write(formatted);

    // ¡Aquí integramos el índice!
    IndexManager idxMgr;
    idxMgr.createIndex(tableName, schm);
    // (internamente, createIndex ya llama a persist())

    // Guardamos el nombre de la tabla en el storageManager
    this->tableName = tableName;
    return true;

  } catch (const std::exception &e) {
    std::cerr << "storageManager::uploadCSV - Error: " << e.what() << '\n';
    return false;
  } catch (...) {
    std::cerr << "storageManager::uploadCSV - Error desconocido\n";
    return false;
  }
}
bool storageManager::is_open() const {
  if (tableName.empty() || schm.fields.size() == 0) {
    return false;
  }
  return true;
}

void storageManager::reset() {
  tableName = "";
  schm = {"", {}};
}

void storageManager::selectall() {
  if (!is_open()) {
    cout << "No se abrio una tabla\n";
    return;
  }
  size_t size = schemas->getRecordSize(tableName);
  // cout<<"SIZE: "<<size<<endl;
  File table(tableName);
  string content = table.accessBlock();
  for (size_t i = 0; i < schm.fields.size(); ++i) {
    cout << schm.fields[i].field_name;
    if (i + 1 < schm.fields.size())
      cout << " | ";
  }
  cout << '\n';
  RecordManagerFixed rm(tableName);
  do {
    content = table.accessBlock();
    auto recs = rm.parseFixedData(content, schm);
    // cout<<recs.size()<<endl;
    for (auto &i : recs) {
      for (auto &f : i) {
        cout << f << "|";
      }
      cout << '\n';
    }
  } while (table.nextBlock());

  table.close();
}

bool storageManager::load(string relationname) {
  if (!schemas->findSchema(relationname)) {
    return false;
  }
  schm = schemas->getSchema(relationname);
  tableName = relationname;
  im.loadIndex(relationname);

  return true;
}

int getFieldIndex(const std::string &name, const Schema &schm) {
  for (size_t i = 0; i < schm.fields.size(); ++i) {
    if (schm.fields[i].field_name == name) {
      return static_cast<int>(i);
    }
  }
  return -1; // No encontrado
}

void storageManager::selectColumns(const vector<string> &cols) {
  if (!is_open()) {
    std::cerr << "[ERROR] No hay ninguna tabla cargada.\n";
    return;
  }

  // Buscar índices de las columnas a mostrar
  vector<int> indices;
  for (const auto &col : cols) {
    int idx = getFieldIndex(col, schm);
    if (idx == -1) {
      std::cerr << "[ERROR] Columna no existe: " << col << "\n";
      return;
    }
    indices.push_back(idx);
  }

  // Imprimir encabezados
  for (size_t i = 0; i < indices.size(); ++i) {
    std::cout << schm.fields[indices[i]].field_name;
    if (i + 1 < indices.size())
      std::cout << " | ";
  }
  std::cout << '\n';

  // Abrir archivo y leer bloque por bloque
  File table(tableName, 'r');
  RecordManagerFixed rm(tableName);
  string content;

  do {
    content = table.accessBlock();
    if (content.empty())
      continue;

    auto recs = rm.parseFixedData(content, schm);

    for (const auto &rec : recs) {
      for (size_t i = 0; i < indices.size(); ++i) {
        std::cout << rec[indices[i]];
        if (i + 1 < indices.size())
          std::cout << " | ";
      }
      std::cout << '\n';
    }

  } while (table.nextBlock());

  table.close();
}

void storageManager::selectWhere(const std::string &col, const std::string &op,
                                 const std::string &val) {
  if (!is_open()) {
    std::cerr << "[ERROR] No hay tabla cargada.\n";
    return;
  }

  int idx = getFieldIndex(col, schm);
  if (idx == -1) {
    std::cerr << "[ERROR] Columna no existe: " << col << "\n";
    return;
  }
  else if (idx == 0 && op == "="/* && op == "<" && op == ">"*/) {
    selectwithindex(col, op, val);
    return;
  }

  File table(tableName, 'r');
  RecordManagerFixed rm(tableName);

  //header tabla
  for (size_t i = 0; i < schm.fields.size(); ++i) {
    std::cout << schm.fields[i].field_name
              << (i + 1 < schm.fields.size() ? " | " : "");
  }
  std::cout << '\n';

  do {
    std::string block = table.accessBlock();
    auto recs = rm.parseFixedData(block, schm);
    for (auto &row : recs) {
      if (compare(trim(row[idx]), op, val, schm.fields[idx].type)) {
        for (auto &f : row) {
          std::cout << (f) << " | ";
        }
        std::cout << '\n';
      }
    }
  } while (table.nextBlock());

  table.close();
}

void storageManager::selectColumnsWhere(const std::vector<std::string> &cols,
                                        const std::string &col,
                                        const std::string &op,
                                        const std::string &val) {
  if (!is_open()) {
    std::cerr << "[ERROR] No hay tabla cargada.\n";
    return;
  }

  int whereIdx = getFieldIndex(col, schm);
  if (whereIdx == -1) {
    std::cerr << "[ERROR] Columna de condición no existe: " << col << "\n";
    return;
  }
  if (whereIdx == 0 && op == "=") {
    selectwithindexcolumns(cols, col, op, val);
    return;
  }

  std::vector<int> colIndices;
  for (auto &c : cols) {
    int idx = getFieldIndex(c, schm);
    if (idx == -1) {
      std::cerr << "[ERROR] Columna no existe: " << c << "\n";
      return;
    }
    colIndices.push_back(idx);
  }

  File table(tableName, 'r');
  RecordManagerFixed rm(tableName);

  for (size_t i = 0; i < colIndices.size(); ++i) {
    std::cout << schm.fields[colIndices[i]].field_name
              << (i + 1 < colIndices.size() ? " | " : "");
  }
  std::cout << '\n';

  do {
    std::string block = table.accessBlock();
    auto recs = rm.parseFixedData(block, schm);
    for (auto &row : recs) {
      if (compare(trim(row[whereIdx]), op, val, schm.fields[whereIdx].type)) {
        for (auto idx : colIndices) {
          std::cout << (row[idx]) << " | ";
        }
        std::cout << '\n';
      }
    }
  } while (table.nextBlock());

  table.close();
}

BPlusTree* storageManager::getTree() {
  return this->im.getIndex();
}

void storageManager::selectwithindex(const std::string &col,
                                     const std::string &op,
                                     const std::string &val) {
  BPlusTree* index = im.getIndex();
  if (!index) {
    std::cerr << "[ERROR] No hay índice cargado.\n";
    return;
  }
  if (op != "=") {
    std::cerr << "[ERROR] El índice solo soporta comparaciones '='.\n";
    return;
  }
  int key;
  try {
    key = std::stoi(val);
  } catch (...) {
    std::cerr << "[ERROR] Valor inválido para búsqueda: " << val << "\n";
    return;
  }
  Value res = index->search(key);
  if (res.position == -1) {
    std::cout << "[INFO] Clave no encontrada: " << key << "\n";
    return;
  }
  std::string content = bufferPool->requestPage(res.position, 'r').substr(4);

  RecordManagerFixed rm(this->tableName);
  std::vector<std::vector<std::string>> recs = rm.parseFixedData(content, schm);
  cout<<"RECS size: "<<recs.size()<<endl;

  for (size_t i = 0; i < schm.fields.size(); ++i) {
    std::cout << schm.fields[i].field_name
              << (i + 1 < schm.fields.size() ? " | " : "\n");
  }

  for (const auto& row : recs) {
    if (row.size() > 0 && stoi(trim(row[0])) == stoi(trim(val))) {
      for (const auto& field : row) {
        std::cout << field << " | ";
      }
      std::cout << '\n';
      return;
    }
  }

  std::cout << "[INFO] Registro no encontrado en la página indicada.\n";
}

void storageManager::selectwithindexcolumns(const std::vector<std::string> &cols,
                                            const std::string &col,
                                            const std::string &op,
                                            const std::string &val) {
  BPlusTree* index = im.getIndex();
  if (!index) {
    std::cerr << "[ERROR] No hay índice cargado.\n";
    return;
  }
  if (op != "=") {
    std::cerr << "[ERROR] El índice solo soporta comparaciones '='.\n";
    return;
  }

  int key;
  try {
    key = std::stoi(val);
  } catch (...) {
    std::cerr << "[ERROR] Valor inválido para búsqueda: " << val << "\n";
    return;
  }

  Value res = index->search(key);
  if (res.position == -1) {
    std::cout << "[INFO] Clave no encontrada: " << key << "\n";
    return;
  }

  std::string content = bufferPool->requestPage(res.position, 'r').substr(4);

  RecordManagerFixed rm(this->tableName);
  std::vector<std::vector<std::string>> recs = rm.parseFixedData(content, schm);

  // Imprimir encabezados solo si son válidos
  std::vector<int> colIndexes;
  for (const auto& name : cols) {
    int idx = getFieldIndex(name, schm);
    if (idx == -1) {
      std::cerr << "[WARN] Columna no encontrada: " << name << "\n";
    } else {
      colIndexes.push_back(idx);
      std::cout << name << " | ";
    }
  }
  std::cout << '\n';

  for (const auto& row : recs) {
    if (row.size() > 0 && stoi(trim(row[0])) == stoi(trim(val))) {
      for (int i : colIndexes) {
        std::cout << row[i] << " | ";
      }
      std::cout << '\n';
      return;
    }
  }

  std::cout << "[INFO] Registro no encontrado en la página indicada.\n";
}

