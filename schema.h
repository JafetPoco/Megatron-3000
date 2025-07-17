#ifndef SCHEM_H
#define SCHEM_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "file.h"

enum FieldType { INT, STRING, DOUBLE };

struct Field {
  std::string field_name;
  size_t size;
  FieldType type; //role
};

class Schema {
private:
  std::vector<Field> fields;
  File schemaFile;
  /*
   * @brief Carga el esquema desde un vector
   * @param fields Vector de campos a cargar
   * @return true si se carga correctamente, false en caso contrario
   * @author Berly Dueñas
   * */
  bool uploadFromVector(std::vector<Field> &fields);
  bool load(std::string &line);
public:
  /*
   * @brief Constructor de la clase Schema, crea un esquema vacío
   * @author Berly Dueñas
   * */
  Schema();

  /* 
   * @brief Constructor de la clase Schema busca el esquema con el nombre de la relación
   * en el fichero de esquema de disco
   * @param relation_name Nombre de la relación
   * @author Berly Dueñas
   * */
  Schema(string relation_name);

  /*
   * @brief cierra el esquema, reinicia al estado inicial
   * mantiene File
   * @author Berly Dueñas
   * */
  void close();

  /*
   * @brief Comprueba si el esquema está abierto
   * @return true si el esquema está abierto, false en caso contrario
   * @author Berly Dueñas
   * */
  bool is_open() const;

  /*
   * @brief 
   * @param Obtener el numero de campos del esquema
   * @return Número de campos del esquema size_t
   * @author Berly Dueñas
   * */
  size_t getLength();

  /*
   * @brief Devuelve los campos del esquema
   * @return Referencia a un vector de campos
   * @author Berly Dueñas
   * */
  std::vector<Field> &getFields() { return fields; }

  /*
   * @brief imprime el Esquema
   * @author Berly Dueñas
   * */
  void printSchema(); 

  /*
   * @brief da el tamaño de un registro en bytes
   * @return El tamaño del registro en bytes
   * */
  size_t getRecordSize() const;

};
#endif
