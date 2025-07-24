#include "storage.h"
#include "file.h"
#include "globals.h"
#include "recordManager.h"
#include "schema.h"
#include <iostream>

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
  size_t size =schemas->getRecordSize(tableName);
  File table(tableName);
  string content = table.accessBlock();
  while (table.nextBlock()) {
    content += table.accessBlock();
  }
  for (size_t i = 0; i < schm.fields.size(); ++i) {
    cout << schm.fields[i].field_name;
    if (i + 1 < schm.fields.size()) cout << " | ";
  }
  cout << '\n';
  for (size_t i = 0; i + size<= content.size(); i += size) {
    cout << content.substr(i, size) << '\n';
  }
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
  vector<int> indices;
  for (const auto& col : cols) {
    int idx = getFieldIndex(col, schm);
    if (idx == -1) {
      std::cerr << "[ERROR] Columna no existe: " << col << "\n";
      return;
    }
    cout<<schm.fields[idx].field_name<<" | ";
    indices.push_back(idx);
  }
  File table(tableName, 'r');
  string content = table.accessBlock();
  while (table.nextBlock()) { content+=table.accessBlock(); }
  RecordManagerFixed rm(tableName);

  auto recs = rm.parseFixedData(content, schm);

  for (size_t i = 0; i<recs.size(); i++) {
    for (auto &j : indices) {
      cout<<recs[i][j]<<' ';
    }
    cout<<"\n";
  }
}

