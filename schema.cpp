#include "schem.h"
#include <iostream>
#include <sstream>
#include "file.h"
Schema::Schema() {}

size_t Schema::getNumFields() { return fields.size(); }

const Field &Schema::getField(size_t index) { return fields.at(index); }

size_t Schema::getRecordSize() {
  size_t total = 0;
  for (const auto &f : fields) {
    total += f.size;
  }
  return total;
}

bool Schema::loadFromSchemaLine(const std::string &line) {
  fields.clear();
  std::stringstream ss(line);
  std::string token;
  std::vector<std::string> parts;

  while (std::getline(ss, token, '#')) {
    if (!token.empty()) {
      parts.push_back(token);
    }
  }

  if ((parts.size() - 1) % 3 != 0) {
    std::cerr << "SCHEMA: Formato inválido: " << line << std::endl;
    return false;
  }

  for (size_t i = 1; i + 2 < parts.size(); i += 3) {
    std::string name = parts[i];
    std::string typeRaw = parts[i + 1];
    std::string sizeStr = parts[i + 2];

    FieldType type;
    if (typeRaw.rfind("long", 0) == 0) {
      type = INT;
    } else if (typeRaw.rfind("string", 0) == 0) {
      type = STRING;
    } else if (typeRaw.rfind("double", 0) == 0) {
      type = DOUBLE;
    } else {
      std::cerr << "Tipo no soportado: " << typeRaw << std::endl;
      continue;
    }

    size_t size = 0;
    try {
      size = std::stoul(sizeStr);
    } catch (...) {
      std::cerr << "Tamaño inválido: " << sizeStr << std::endl;
      continue;
    }

    fields.push_back({name, type, size});
  }

  // std::cerr << "SCHEMA: field size: " << fields.size() << std::endl;
  return true;
}

bool Schema::loadFromFile(const std::string &filename, std::string &tableName) {
  File in(filename);
  //std::ifstream in(filename);
  /*
  if (!in) {
    std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
    return false;
  }
  */

  std::string line;
  std::stringstream inS(in.readAll());
  while (std::getline(inS, line)) {
    std::stringstream ss(line);
    std::cout<<line<<std::endl;
    std::string name;
    getline(ss, name, '#');
    if(name == tableName){
      return loadFromSchemaLine(line);
    }
  }

  std::cerr << "No existe la tabla '" << tableName << "' en el archivo." << std::endl;
  return false;
}

void Schema::printSchema() {
  std::cout << "Esquema con " << fields.size() << " campos:\n";
  for (const auto &f : fields) {
    std::string typeStr;
    switch (f.type) {
    case INT:
      typeStr = "LONG";
      break;
    case STRING:
      typeStr = "STRING";
      break;
    case DOUBLE:
      typeStr = "DOUBLE";
      break;
    }
    std::cout << "- " << f.name << " (" << typeStr << ", " << f.size
              << " bytes)\n";
  }
}
