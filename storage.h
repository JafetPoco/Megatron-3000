#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H
#include "recordManager.h"
#include "schema.h"
class storageManager {
  string tableName; //current
  RecordManager* rm;
public:
  storageManager();
  bool uploadCSV(string csvFilename, string tableName);
  bool load(string relationname);
  void reset();
  
  void select();
};
#endif
