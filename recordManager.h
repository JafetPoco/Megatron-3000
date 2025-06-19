#ifndef RECORD_MAN_H
#define RECORD_MAN_H

#include "schem.h"
#include <string>

class RecordManager {
protected:
  Schema *sh;
public:
  virtual void addToSchema(std::string firstsRow, std::string tableName) = 0;
  virtual void select(std::string tableName) = 0;
  //virtual void where() = 0;
  //virtual void insert() = 0;
  virtual void readCSV(std::string file) = 0; 
  ~RecordManager() = default;
};

class RecordManagerFixed : public RecordManager{
private:
  std::string formatRow(std::string row);
  void printHeader(std::string file);
public:
  RecordManagerFixed();
  void addToSchema(std::string firstsRow, std::string tableName);
  void readCSV(std::string file) override;
  ~RecordManagerFixed() = default;
  void select(std::string tableName) override;
};

class RecordManagerVariable : public RecordManager{
public:
  std::string formatRow(std::string row);
  RecordManagerVariable();
  void select(std::string tableName) override;
  void addToSchema(std::string firstsRow, std::string tableName) override;
  void readCSV(std::string file) override;
  ~RecordManagerVariable() = default;
};

#endif //RECORD_MAN_H
