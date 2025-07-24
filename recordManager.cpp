#include "recordManager.h"
#include "file.h"
#include "globals.h"
#include "schema.h"
#include <fstream>
#include <iostream>
#include <vector>

std::string RecordManagerFixed::formatRow(vector<string> row, Schema &schema) {
  stringstream ss;
  for (size_t i = 0; i < schema.fields.size(); ++i) {
    string value = (i < row.size()) ? row[i] : "";
    size_t fieldSize = schema.fields[i].size;

    // Adjust the string to be exactly fieldSize characters
    if (value.length() < fieldSize) {
      value.append(fieldSize - value.length(), ' ');
    } else if (value.length() > fieldSize) {
      value = value.substr(0, fieldSize);
    }

    ss << value;
  }
  return ss.str();
}

std::vector<std::string> RecordManagerFixed::formatRows(const std::vector<Record> &records,
                               Schema &schema) {
  std::vector<std::string> formatted;
  for (const auto &record : records) {
    formatted.push_back(formatRow(record, schema));
  }
  return formatted;
}

void RecordManagerFixed::write(vector<string> records, string filename) {}
