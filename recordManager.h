#ifndef RECORD_MAN_H
#define RECORD_MAN_H

#include "schema.h"
#include "hash.h"
#include <vector>

using namespace std;

class RecordManager {
public:
  virtual std::string formatRow(vector<string>, Schema& sizes) = 0;
  virtual std::vector<std::string> formatRows(const std::vector<Record>& records, Schema& schema) = 0;
  RecordManager() = default;
  ~RecordManager() = default;
};

class RecordManagerFixed : public RecordManager{
private:
  string tableName;
  void printHeader(std::string file);
public:
  std::string formatRow(vector<string>, Schema& sizes) override;
  std::vector<std::string> formatRows(const std::vector<Record>& records, Schema& schema) override;
  std::vector<Record> parseFixedData(const std::string& data, const Schema& schema);
  void write(vector<string> records);
  RecordManagerFixed(string tableName);
};

// class RecordManagerVariable : public RecordManager{
// private:
//   std::string formatRow(vector<string>, Schema& sizes) override;
//   void printHeader(std::string file);
// public:
//   RecordManagerVariable();
//   ~RecordManagerVariable() = default;
//   void addToSchema(std::string firstsRow, std::string tableName);
//   void select(std::string tableName) override;
// };

#endif //RECORD_MAN_H
