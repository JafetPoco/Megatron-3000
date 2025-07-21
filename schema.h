#ifndef SCHEM_H
#define SCHEM_H

#include "file.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum FieldType { INT, STRING, DOUBLE };

struct Field {
  std::string field_name;
  size_t size = 0;
  FieldType type = FieldType::INT;
};

struct Schema {
  string schemaName;
  std::vector<Field> fields;
};

class SchemaManager {
private:
  std::vector<Schema> schemas;
  /*
   * @brief Carga el esquema desde un vector
   * @param fields Vector de campos a cargar
   * @return true si se carga correctamente, false en caso contrario
   * @author Berly Dueñas
   * */
  bool uploadFromVector(std::vector<Field> &fields);
  std::vector<Schema> parseSchemas(const std::string &input);

public:
  /*
   * @brief Constructor de la clase Schema, crea un esquema vacío
   * @author Berly Dueñas
   * */
  SchemaManager();

  bool findSchema(string schemaName);

  /*
   * @brief
   * @param Obtener el numero de campos del esquema
   * @return Número de campos del esquema size_t
   * @author Berly Dueñas
   * */
  size_t getLength(string schemaName);

  /*
   * @brief Devuelve los campos del esquema
   * @return Referencia a un vector de campos
   * @author Berly Dueñas
   * */
  std::vector<Field> &getFields(string schemaName);

  /*
   * @brief imprime el Esquema
   * @author Berly Dueñas
   * */
  void printSchema();

  /*
   * @brief da el tamaño de un registro en bytes
   * @return El tamaño del registro en bytes
   * */
  size_t getRecordSize(string schemaName) const;
};
#endif
