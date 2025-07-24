#ifndef RECORD_MAN_H
#define RECORD_MAN_H

#include "schema.h"
#include "hash.h"
#include <vector>

using namespace std;

class RecordManager {
protected:
  virtual std::string formatRow(vector<string>) = 0;
public:
  virtual void select(std::string tableName) = 0;
  // virtual void where() = 0;
  // virtual void insert() = 0;
  ~RecordManager() = default;
};

class RecordManagerFixed : public RecordManager{
private:
  std::string formatRow(vector<string>) override;
  void printHeader(std::string file);
public:
  RecordManagerFixed(string tableName);
  ~RecordManagerFixed() = default;
  void select(std::string tableName) override;
};

class RecordManagerVariable : public RecordManager{
private:
  std::string formatRow(vector<string>) override;
  void printHeader(std::string file);
public:
  RecordManagerVariable();
  ~RecordManagerVariable() = default;
  void addToSchema(std::string firstsRow, std::string tableName);
  void select(std::string tableName) override;
};

#endif //RECORD_MAN_H
