#ifndef RECORD_MAN_H
#define RECORD_MAN_H

#include "schem.h"

class RecordManager {
protected:
  Schema *sh;
public:
  virtual void addToSchema(std::string firstsRow, std::string tableName) = 0;
  //virtual void select() = 0;
  //virtual void where() = 0;
  //virtual void insert() = 0;
  virtual void readCSV(std::string file) = 0; 
  ~RecordManager() = default;
};

class RecordManagerFixed : public RecordManager{
public:
  RecordManagerFixed();
  void addToSchema(std::string firstsRow, std::string tableName);
  void readCSV(std::string file) override;
  ~RecordManagerFixed() = default;
  void printHeader(std::string file);
};

#endif //RECORD_MAN_H