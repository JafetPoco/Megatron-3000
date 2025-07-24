#ifndef RECORD_MAN_H
#define RECORD_MAN_H

#include "schema.h"
#include "hash.h"
#include <vector>

using namespace std;

class RecordManager {
protected:
  virtual std::string formatRow(vector<string>, Schema& sizes) = 0;
public:
  ~RecordManager() = default;
};

class RecordManagerFixed : public RecordManager{
private:
  string tableName;
  void printHeader(std::string file);
  std::string formatRow(vector<string>, Schema& sizes) override;
public:
  std::vector<std::string> formatRows(const std::vector<Record>& records, Schema& schema);
  void write(vector<string> records, string filename);
  RecordManagerFixed(string tableName) : tableName(tableName){}
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
