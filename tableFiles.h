#ifndef TABLE_FILES_H
#define TABLE_FILES_H

#include <iostream>
#include <map>
#include <string>

class TableFiles {
private:
  std::map<std::string, ssize_t> table;

  /**
   * @brief Lee el directorio del disco, y crea un mapa con el nombre de un archivo y el ID del bloque donde empieza
   * @author Jafet Poco
   */
  void loadTable();
  /**
   * @brief Escribe los cambios realizados en el disco
   * @author Jafet Poco
   */
  void saveTable();

public:
  /**
   * @brief Constructor: Lee la tabla de archivos
   * @author Jafet Poco
   */
  TableFiles();

  /**
   * @brief Busca un archivo por el nombre dentro del mapa (directorio)
   * @param name Nombre del archivo
   * @return ssize_t id del bloque inicio si el archivo existe o -1 de lo contrario
   * @author Jafet Poco
   */
  ssize_t findFile(std::string name);
  /**
   * @brief Imprime la tabla de archivos
   * @author Jafet Poco
   */
  void showTable();
  /**
   * @brief Agrega un archivo al mapa (directorio)
   * @param name Nombre del archivo
   * @return ssize_t Id del bloque
   * @author Jafet Poco
   */
  ssize_t addFile(std::string name);
};

#endif // TABLE_FILES_H
