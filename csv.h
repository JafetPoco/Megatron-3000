#pragma once
#include "schema.h"
class CSVProcessor {
public:
  explicit CSVProcessor(const std::string& filename);
  void process();
  const std::vector<Field>& getFields() const;
  const std::vector<Record>& getData() const;
  ~CSVProcessor() = default;

private:
  std::string            filename_;
  std::vector<Field>     fields_;
  std::vector<Record> records_;

  std::vector<std::string> parseLine(const std::string& line) const;
  FieldType                inferValueType(const std::string& value) const;
};
