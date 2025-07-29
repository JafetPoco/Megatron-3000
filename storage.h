#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H
#include "bptree2.h"
#include "recordManager.h"
#include "schema.h"
#include "index.h"
#include <memory>
class storageManager {
  string tableName; //current
  RecordManager* rm;
  Schema schm;
  IndexManager im;
public:
  storageManager() = default;
  ~storageManager();
  bool uploadCSV(string csvFilename, string tableName);
  bool load(string relationname);
  void reset();
  bool is_open() const;
  
  void selectall();
  void selectColumns(const vector<string>& cols);
  void selectWhere(const string& col,const string& op, const string& val);

  void selectColumnsWhere(const vector<string>& cols,const string& col, const string& op, const string& val);
};
#endif
