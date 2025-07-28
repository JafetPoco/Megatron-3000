#include "recordManager.h"
#include "file.h"
#include "globals.h"
#include "schema.h"
#include <fstream>
#include <iostream>
#include <vector>

RecordManagerFixed::RecordManagerFixed(string tableName)
    : tableName(tableName) {
  File file(tableName);
  file.close();
}
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

std::vector<std::string>
RecordManagerFixed::formatRows(const std::vector<Record> &records,
                               Schema &schema) {
  std::vector<std::string> formatted;
  for (const auto &record : records) {
    formatted.push_back(formatRow(record, schema));
  }
  return formatted;
}

void RecordManagerFixed::write(std::vector<std::string> records) {
  if (records.empty()) {
    std::cerr << "RM: no se escribe nada, records vacío\n";
    return;
  }

  File file(tableName, 'w'); // Usa tableName como nombre del archivo
  int recordSize = records[0].size();
  int blockCapacity = file.getCapacity();
  int maxPerBlock = blockCapacity / recordSize;

  size_t totalRecords = records.size();
  size_t written = 0;

  while (written < totalRecords) {
    // Calcular cuántos caben en este bloque
    size_t count =
        std::min(static_cast<size_t>(maxPerBlock), totalRecords - written);

    // Unir los registros para este bloque
    std::string blockData;
    for (size_t i = 0; i < count; ++i) {
      blockData += records[written + i];
    }

    // Escribir en el bloque actual
    std::string &block = file.accessBlock();
    block = blockData;

    written += count;

    // Si aún hay registros por escribir
    if (written < totalRecords) {
      if (!file.nextBlock()) {
        file.addBlock();
      }
    }
  }

  file.close();
}

std::vector<Record> RecordManagerFixed::parseFixedData(std::string data,
                                                       const Schema &schema) {
  std::vector<Record> records;
  while (!data.empty() && data.back() == '\0') {
    data.pop_back();
  }

  size_t recordSize = schemas->getRecordSize(schema.schemaName);
  size_t recordCount = data.size() / recordSize;
  // cout<<data.size()<<"\n\n*************\n\n"<<recordCount<<endl;

  for (size_t i = 0; i < recordCount; ++i) {
    Record record;
    size_t baseOffset = i * recordSize;
    size_t fieldOffset = 0;

    for (const auto &field : schema.fields) {
      std::string fieldValue = data.substr(baseOffset + fieldOffset, field.size);
      // Remove trailing spaces
      // fieldValue.erase(fieldValue.find_last_not_of(' ') + 1);
      record.push_back(fieldValue);
      fieldOffset += field.size;
    }

    records.push_back(record);
  }

  return records;
}
