#include "csv.h"
/*
 * CSVProcessor implementation 
 * 
 * */

CSVProcessor::CSVProcessor(const std::string& filename)
  : filename_(filename)
{}

void CSVProcessor::process() {
  std::ifstream in(filename_);
  if (!in.is_open()) {
    throw std::runtime_error("No se pudo abrir: " + filename_);
  }

  std::string line;
  // Leer cabecera
  if (!std::getline(in, line)) return;
  auto headers = parseLine(line);
  fields_.resize(headers.size());
  for (size_t i = 0; i < headers.size(); ++i) {
    fields_[i].field_name = headers[i];
  }

  // Leer resto de filas
  records_.clear();
  while (std::getline(in, line)) {
    auto values = parseLine(line);
    records_.push_back(values);  // no need to copy again later
    for (size_t i = 0; i < values.size() && i < fields_.size(); ++i) {
      const auto& v = values[i];
      fields_[i].size = std::max(fields_[i].size, v.length());
      fields_[i].type = std::max(fields_[i].type, inferValueType(v));
    }
  }
}

const std::vector<Record>& CSVProcessor::getData() const {
    return records_;
}


const std::vector<Field>& CSVProcessor::getFields() const {
  return fields_;
}

std::vector<std::string> CSVProcessor::parseLine(const std::string& line) const {
  std::vector<std::string> result;
  std::string field;
  bool        inQuotes = false;

  for (size_t i = 0; i <= line.size(); ++i) {
    char c = (i < line.size()) ? line[i] : ',';
    if (c == '"') {
      if (inQuotes && i+1 < line.size() && line[i+1] == '"') {
        field += '"';
        ++i;
      } else {
        inQuotes = !inQuotes;
      }
    }
    else if (c == ',' && !inQuotes) {
      result.push_back(field);
      field.clear();
    }
    else {
      field += c;
    }
  }
  return result;
}

FieldType CSVProcessor::inferValueType(const std::string& value) const {
  if (value.empty()) {
    return FieldType::INT;  // No alteramos el tipo si está vacío
  }

  // 1) Intentar INT
  try {
    size_t idx = 0;
    std::stol(value, &idx);
    if (idx == value.size()) {
      return FieldType::INT;
    }
  } catch (...) {
    // no es INT
  }

  // 2) Intentar DOUBLE
  try {
    size_t idx = 0;
    std::stod(value, &idx);
    if (idx == value.size()) {
      return FieldType::DOUBLE;
    }
  } catch (...) {
    // no es DOUBLE
  }

  // 3) Si falla todo, STRING
  return FieldType::STRING;
}


