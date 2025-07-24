#include "storage.h"
#include "file.h"
#include "globals.h"
#include "recordManager.h"
#include "schema.h"
#include <iostream>

bool compare(const string& left, const string& op, const string& right, FieldType type) {
  if (type == FieldType::INT) {
    int l = stoi(left), r = stoi(right);
    if (op == "=") return l == r;
    if (op == "!=") return l != r;
    if (op == "<") return l < r;
    if (op == "<=") return l <= r;
    if (op == ">") return l > r;
    if (op == ">=") return l >= r;
  } else if (type == FieldType::DOUBLE) {
    double l = stod(left), r = stod(right);
    if (op == "=") return l == r;
    if (op == "!=") return l != r;
    if (op == "<") return l < r;
    if (op == "<=") return l <= r;
    if (op == ">") return l > r;
    if (op == ">=") return l >= r;
  } else {
    if (op == "=") return left == right;
    if (op == "!=") return left != right;
    if (op == "<") return left < right;
    if (op == "<=") return left <= right;
    if (op == ">") return left > right;
    if (op == ">=") return left >= right;
  }
  return false;
}

bool storageManager::uploadCSV(string csvfile, string tableName) {
  try {
    cout<<"Subiendo csv "<<csvfile<<" con nombre "<<tableName<<endl;
    schemas->uploadCsv(csvfile, tableName);
    RecordManagerFixed rm(tableName);

    CSVProcessor csv(csvfile);
    vector<Record> test = csv.getData();

    schm= schemas->getSchema(tableName);
    auto formatted = rm.formatRows(test, schm);
    rm.write(formatted);
    tableName = tableName;

    return true;
  } catch (const std::exception& e) {
    std::cerr << "storageManager::uploadCSV - Error: " << e.what() << std::endl;
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
  schm = { "", {} };
}

void storageManager::selectall() {
  if (!is_open()) {
    cout<<"No se abrio una tabla\n";
    return;
  }
  size_t size = schemas->getRecordSize(tableName);
  cout<<"SIZE: "<<size<<endl;
  File table(tableName);
  string content = table.accessBlock();
  // cout<<content<<endl;
  for (size_t i = 0; i < schm.fields.size(); ++i) {
    cout << schm.fields[i].field_name;
    if (i + 1 < schm.fields.size()) cout << " | ";
  }
  cout << '\n';
  RecordManagerFixed rm(tableName);
  do {
    content = table.accessBlock();
    if (content[content.size()-1] == '\n') content.pop_back();
    auto recs = rm.parseFixedData(content, schm);
    for (auto&i : recs) {
      for (auto& f : i) {
        cout<< f<<" ";
      }
      cout<<endl;
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
  return true;
}

int getFieldIndex(const std::string& name, const Schema& schm) {
  for (size_t i = 0; i < schm.fields.size(); ++i) {
    if (schm.fields[i].field_name == name) {
      return static_cast<int>(i);
    }
  }
  return -1; // No encontrado
}

void storageManager::selectColumns(const vector<string>& cols) {
  if (!is_open()) {
    std::cerr << "[ERROR] No hay ninguna tabla cargada.\n";
    return;
  }

  // Buscar índices de las columnas a mostrar
  vector<int> indices;
  for (const auto& col : cols) {
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
    if (i + 1 < indices.size()) std::cout << " | ";
  }
  std::cout << '\n';

  // Abrir archivo y leer bloque por bloque
  File table(tableName, 'r');
  RecordManagerFixed rm(tableName);
  string content;

  do {
    content = table.accessBlock();
    if (content.empty()) continue;

    auto recs = rm.parseFixedData(content, schm);

    for (const auto& rec : recs) {
      for (size_t i = 0; i < indices.size(); ++i) {
        std::cout << rec[indices[i]];
        if (i + 1 < indices.size()) std::cout << " | ";
      }
      std::cout << '\n';
    }

  } while (table.nextBlock());

  table.close();
}

void storageManager::selectWhere(const string& col, const string& op, const string& val) {
  if (!is_open()) {
    std::cerr << "[ERROR] No hay tabla cargada.\n";
    return;
  }

  int idx = getFieldIndex(col, schm);
  if (idx == -1) {
    std::cerr << "[ERROR] Columna no existe: " << col << "\n";
    return;
  }

  indexManager.loadAllIndices(tableName);
  auto& index = indexManager.getIndex(tableName, col);
  vector<int> matchIndices = index.search(op, val);
  File table(tableName, 'r');
  string content = table.accessBlock();
  while (table.nextBlock()) content += table.accessBlock();

  RecordManagerFixed rm(tableName);
  auto recs = rm.parseFixedData(content, schm);

  for (const auto& f : schm.fields) cout << f.field_name << " | ";
  cout << '\n';

  for (size_t i = 0; i < recs.size(); ++i) {
    if (matchIndices.empty() || compare(recs[i][idx], op, val, schm.fields[idx].type)) {
      for (const auto& val : recs[i]) cout << val << " | ";
      cout << '\n';
    }
  }
}

void storageManager::selectColumnsWhere(const vector<string>& cols, const string& col, const string& op, const string& val) {
  if (!is_open()) {
    std::cerr << "[ERROR] No hay tabla cargada.\n";
    return;
  }

  int whereIdx = getFieldIndex(col, schm);
  if (whereIdx == -1) {
    std::cerr << "[ERROR] Columna de condición no existe: " << col << "\n";
    return;
  }

  vector<int> colIndices;
  for (const auto& c : cols) {
    int idx = getFieldIndex(c, schm);
    if (idx == -1) {
      std::cerr << "[ERROR] Columna no existe: " << c << "\n";
      return;
    }
    colIndices.push_back(idx);
  }

  indexManager.loadAllIndices(tableName);
  auto& index = indexManager.getIndex(tableName, col);
  vector<int> matchIndices = index.search(op, val);

  File table(tableName, 'r');
  string content = table.accessBlock();
  while (table.nextBlock()) content += table.accessBlock();

  RecordManagerFixed rm(tableName);
  auto recs = rm.parseFixedData(content, schm);

  for (size_t i = 0; i < colIndices.size(); ++i) {
    cout << schm.fields[colIndices[i]].field_name;
    if (i + 1 < colIndices.size()) cout << " | ";
  }
  cout << '\n';

  for (size_t i = 0; i < recs.size(); ++i) {
    if (matchIndices.empty() || compare(recs[i][whereIdx], op, val, schm.fields[whereIdx].type)) {
      for (auto idx : colIndices) cout << recs[i][idx] << " | ";
      cout << '\n';
    }
  }
}
