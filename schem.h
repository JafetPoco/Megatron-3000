#ifndef SCHEM_H
#define SCHEM_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum FieldType { INT, STRING, DOUBLE };

struct Field {
  std::string name;
  FieldType type;
  size_t size;
};

class Schema {
private:
  std::vector<Field> fields;
  bool loadFromSchemaLine(const std::string &line);
public:
  Schema();

  size_t getNumFields();
  const Field &getField(size_t index);

  size_t getRecordSize();
  bool loadFromLine(const std::string &line);

  bool loadFromFile(const std::string &filename, std::string &fileName);
  void printSchema();
  
};


#endif
