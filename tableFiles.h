#ifndef TABLE_FILES_H
#define TABLE_FILES_H

#include<map>
#include<iostream>
#include "disk.h"

class TableFiles{
private:
  std::map<std::string, size_t> table;
public:
  TableFiles(Disk *&disk);
  bool findFile(std::string file, size_t *capacity);
  void showTable();
  void saveChanges(Disk *&disk);
  void addFile(std::string nameFile, size_t id);
};

#endif 