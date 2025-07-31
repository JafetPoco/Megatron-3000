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
  void selectwithindexcolumns(const std::vector<std::string> &cols,
                                            const std::string &col,
                                            const std::string &op,
                                            const std::string &val);
  void selectwithindex(const std::string &col,
                                     const std::string &op,
                                     const std::string &val);
public:
  storageManager();
  ~storageManager();
  bool uploadCSV(string csvFilename, string tableName);
  bool load(string relationname);
  void reset();
  bool is_open() const;
  
  void selectall();
  void selectColumns(const vector<string>& cols);
  void selectWhere(const string& col,const string& op, const string& val);

  void selectColumnsWhere(const vector<string>& cols,const string& col, const string& op, const string& val);

  BPlusTree* getTree();
};
#endif
