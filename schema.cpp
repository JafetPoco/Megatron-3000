#include "schema.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include "file.h"

#define DEBUG
#define VERBOSE

std::vector<std::string> splitString(std::string s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s); 

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

Schema::Schema() {}

Schema::Schema(std::string relation_name) {
  File in("schema");
  std::string content = in.accessBlock();
  while (in.nextBlock()) {
    content += in.accessBlock();
  }
  in.close();
#ifdef DEBUG
  std::cout << "SCHEMA: " << content << std::endl;
#endif
  if (content.size() == 0) {
    std::cout<<"No se obtuvo informacion del archivo";
    return;
  }
  // auto rawlines = splitString(content, '\n');
  
  // std::vector<std::vector<string>> relations;
  // for (auto& i : rawlines) {
  //   std::cout<<i<<std::endl;
  //   relations.push_back( splitString(i, '#') );
  // }

}

size_t Schema::getLength() { return fields.size(); }

/*
INPUT: cabecera de una tabla
Me crea datos tipo Field con los datos de cada campo
(nombre del campo, tipo de dato y tamaño)
Autor: Berly Dueñas
*/

bool Schema::load(std::string &line) {
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

    fields.push_back({name, size, type});
  }

  // std::cerr << "SCHEMA: field size: " << fields.size() << std::endl;
  return true;
}

/*
INPUT: Nombre del archivo y Nombre de la tabla
Busca en schema la cabecera de la tabla indicada
Autor: Berly Dueñas 
*/

//bool Schema::loadFromFile(const std::string &filename, std::string &tableName) {
//  File in(filename);
//  //std::ifstream in(filename);
//  /*
//  if (!in) {
//    std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
//    return false;
//  }
//  */

//  std::string line;
//  std::stringstream inS(in.readAll());
//  while (std::getline(inS, line)) {
//    std::stringstream ss(line);
//    std::string name;
//    getline(ss, name, '#');
//    if(name == tableName){
//      return loadFromSchemaLine(line);
//    }
//  }

//  std::cerr << "No existe la tabla '" << tableName << "' en el archivo." << std::endl;
//  return false;
//}

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
    std::cout << "- " << f.field_name << " (" << typeStr << ", " << f.size
              << " bytes)\n";
  }
}
