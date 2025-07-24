#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H
#include "recordManager.h"
#include "schema.h"
class storageManager {
  string tableName;
  RecordManager* rm;
  public:
    storageManager(string name);
    void select();
};
#endif
