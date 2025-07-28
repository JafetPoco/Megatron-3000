#include "csv.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <sstream>

// trim helper
static inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

CSVProcessor::CSVProcessor(const std::string& filename)
  : filename_(filename)
{}

void CSVProcessor::process() {
  std::ifstream in(filename_);
  if (!in.is_open()) {
    throw std::runtime_error("No se pudo abrir: " + filename_);
  }

  std::string line;
  // 1) Leer cabecera
  if (!std::getline(in, line)) return;
  auto headers = parseLine(line);

  // 2) Inicializar campos
  fields_.clear();
  fields_.resize(headers.size());
  for (size_t i = 0; i < headers.size(); ++i) {
    fields_[i].field_name = trim(headers[i]);
    fields_[i].size       = 0;
    fields_[i].type       = FieldType::INT;  // arranca como INT
  }

  // 3) Leer resto de filas
  records_.clear();
  while (std::getline(in, line)) {
    auto rawValues = parseLine(line);
    records_.push_back(rawValues);

    // para cada celda...
    for (size_t i = 0; i < rawValues.size() && i < fields_.size(); ++i) {
      std::string v = trim(rawValues[i]);
      if (v.empty()) {
        // no inferimos nada en celdas vacías
        continue;
      }
      // actualizar tamaño de ancho de columna
      fields_[i].size = std::max(fields_[i].size, v.length());
      // inferir tipo y actualizar sólo si es "mayor"
      FieldType inferred = inferValueType(v);
      if (inferred > fields_[i].type) {
        fields_[i].type = inferred;
      }
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
  // 1) INT
  try {
    size_t idx = 0;
    std::stol(value, &idx);
    if (idx == value.size()) {
      return FieldType::INT;
    }
  } catch (...) { }

  // 2) DOUBLE
  try {
    size_t idx = 0;
    std::stod(value, &idx);
    if (idx == value.size()) {
      return FieldType::DOUBLE;
    }
  } catch (...) { }

  // 3) STRING
  return FieldType::STRING;
}
