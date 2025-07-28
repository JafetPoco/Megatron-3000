#pragma once
#include "schema.h"
class CSVProcessor {
public:
  explicit CSVProcessor(const std::string& filename);
  void process();
  const std::vector<Field>& getFields() const;
  std::vector<Record> getData();

private:
  std::string            filename_;
  std::vector<Field>     fields_;

  std::vector<std::string> parseLine(const std::string& line) const;
  FieldType                inferValueType(const std::string& value) const;
};
