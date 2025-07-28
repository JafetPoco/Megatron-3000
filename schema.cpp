#include "csv.h"
#include "schema.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include "bufPool.h"
#include "file.h"

// #define DEBUG
// #define VERBOSE
#define BOLD        "\033[1m"
#define UNDERLINE   "\033[4m"
std::string fieldTypeToString(FieldType type) {
  switch (type) {
    case INT:
      return "int";
    case STRING:
      return "string";
    case DOUBLE:
      return "double";
    default:
      return "UNKNOWN";
  }
}
FieldType stringToFieldType(std::string s) {
    if (s == "int") {
        return FieldType::INT;
    }
    else if (s == "string" || s == "str" || s == "varchar") {
        return FieldType::STRING;
    }
    else if (s == "double" || s == "float" || s == "real") {
        return FieldType::DOUBLE;
    }
    else {
        throw std::invalid_argument("Unknown FieldType string: '" + s + "'");
    }
}
FieldType parseType(const std::string& value) {
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

std::vector<std::string> splitString(std::string s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s); 

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

SchemaManager::SchemaManager() {
  File schemafile("schema", 'r');
  string content=schemafile.accessBlock();
  while (schemafile.nextBlock()) {
    content += schemafile.accessBlock();
  }
#ifdef DEBUG
  std::cout<<"SCHEMA: loading\n"<<content<<std::endl;
#endif
  schemas = parseSchemas(content);
}

std::vector<Schema> SchemaManager::parseSchemas(const std::string &input) {
  std::vector<Schema> schemas;
  std::istringstream stream(input);
  std::string line;

  while (std::getline(stream, line)) {
    if (line.empty())
      continue;

    std::istringstream linestream(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(linestream, token, '#')) {
      tokens.push_back(token);
    }

    if (tokens.size() < 4 || (tokens.size() - 1) % 3 != 0) {
      // invalid schema line: must have 1 table name + N × (name/type/size)
      continue;
    }

    Schema schema;
    schema.schemaName = tokens[0];

    for (size_t i = 1; i + 2 < tokens.size(); i += 3) {
      Field f;
      f.field_name = tokens[i];
      f.type = stringToFieldType(tokens[i+1]);
      std::cout<<fieldTypeToString(f.type)<<' ';
      f.size = std::stoi(tokens[i + 2]);
      schema.fields.push_back(f);
    }

    schemas.push_back(std::move(schema));
  }

  return schemas;
}

bool SchemaManager::findSchema(string schemaName) {
  for (auto& i : schemas) {
    if (i.schemaName == schemaName) {
      return true;
    }
  }
  return false;
}

Schema SchemaManager::getSchema(string schemaName) {
  for (auto& i : schemas) {
    if (i.schemaName == schemaName) {
      return i;
    }
  }
  // throw std::runtime_error("Schema not found: " + schemaName);
  return { "null" , {} };
}

void SchemaManager::printSchema(){
  for (auto& i : schemas) {
    std::cout << BOLD << GREEN << i.schemaName << "\n" << RESET;
    for (auto& j : i.fields) {
      std::cout << j.field_name << " (" << j.size << ") " 
                << fieldTypeToString(j.type) << '\n';
    }
  }
}

bool SchemaManager::uploadCsv(string filename, string newSchemaName) {
  CSVProcessor csv(filename);
  csv.process();
  Schema schema;
  schema.schemaName = newSchemaName;
  schema.fields = csv.getFields();

  if (schema.fields.size() == 0) {
    std::cerr<<"NO SE PUDO AGREGAR NO HAY CONTENIDO\n";
    return false;
  }

  if (findSchema(schema.schemaName)) {
    std::cout<<"[INFO] Ya existe una relacion con ese nombre\n";
    return false;
  }

  schemas.push_back(schema);
  persist();
  return true;
}

std::vector<std::string> splitStringChunks(const std::string& input, size_t chunkSize) {
    std::vector<std::string> result;
    size_t len = input.size();

    for (size_t i = 0; i < len; i += chunkSize) {
        result.push_back(input.substr(i, chunkSize));
    }

    return result;
}

void SchemaManager::persist() {
  string writeCont;

  // Generar contenido serializado
  for (auto &schema : schemas) {
    std::ostringstream oss;
    oss << schema.schemaName;
    for (const auto& field : schema.fields) {
      oss << "#" << field.field_name
          << "#" << fieldTypeToString(field.type)
          << "#" << field.size;
    }
    writeCont += oss.str() + "\n";
  }
  // std::cout<<writeCont<<std::endl;

  // Crear archivo
  File schemafile("schema", 'w');

  // Dividir en chunks
  auto chunks = splitStringChunks(writeCont, schemafile.getCapacity());
  // std::cout<<"chunk size: "<<chunks.size()<<std::endl;

  // Escribir en bloques
  for (size_t i = 0; i < chunks.size(); ++i) {
    string& payload = schemafile.accessBlock();
    payload = chunks[i];

    // Si hay más por escribir, avanza o agrega nuevo bloque
    if (i + 1 < chunks.size()) {
      if (!schemafile.nextBlock()) {
        schemafile.addBlock();
      }
    }
  }

  schemafile.close();
}
size_t SchemaManager::getRecordSize(string schemaName) {
  Schema schm = getSchema(schemaName);
  size_t size = 0;
  for (auto& i: schm.fields) {
    size += i.size;
  }
  return size;
}
