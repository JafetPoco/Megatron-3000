#ifndef TABLE_FILES_H
#define TABLE_FILES_H

#include <iostream>
#include <map>
#include <string>

class TableFiles {
private:
  std::map<std::string, ssize_t> table;

  void loadTable();
  void saveTable();

public:
  TableFiles();

  ssize_t findFile(std::string name);
  void showTable();
  ssize_t addFile(std::string name);
};

#endif // TABLE_FILES_H
