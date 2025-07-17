#ifndef FILE_H
#define FILE_H

#include <cstddef>
#include <string>
#include <vector>
#include "block.h"
#include "disk.h"
#include "tableFiles.h"
#include "freeBlockMan.h"
#include "bufPool.h"

class File{
private:
  std::string fileName;
  static ssize_t capacity;
  std::vector<BlockID> blocks;
  BlockID currentBlockID;
  char mode;
  std::string payload;

public:

  /* * Constructor por defecto.
   * @brief Crea un objeto File sin inicializar.
   * @author Jafet Poco
   * @note Este constructor no abre ningún archivo.
   */
  File();

  /* * Constructor por defecto y con nombre de archivo y modo.
   * @brief Crea un objeto File con el nombre de archivo y modo especificados.
   * @param fileName Nombre del archivo a abrir.
   * @author Berly Dueñas
   */
  File(std::string fileName, char mode = 'r');

  /* * Constructor que carga un archivo existente.
   * @brief Crea un objeto File y carga el archivo especificado.
   * @param fileName Nombre del archivo a cargar.
   * @author Jafet Poco
   */
  bool open(std::string fileName, char mode='r');

  /* * Destructor.
   * @brief Cierra el archivo si está abierto.
   * @author Jafet Poco
   */
  bool close();

  /* * Carga un bloque del archivo.
   * @brief Carga el bloque especificado en el objeto File.
   * @param blockID ID del bloque a cargar.
   * @author Jafet Poco
   */
  std::string& accessBlock();

  /* * Verifica si el archivo está abierto.
   * @brief Comprueba si el archivo está abierto.
   * @return true si el archivo está abierto, false en caso contrario.
   * @author Jafet Poco
   */
  bool isOpen() const;

  /* * Obtiene el nombre del archivo.
   * @brief Devuelve el nombre del archivo asociado al objeto File.
   * @return Nombre del archivo.
   * @author Berly Dueñas
   */
  string getFilename() const;

  /*
  * @brief Obtiene el siguiente bloque del archivo.
  * @return ID del siguiente bloque del archivo.
  * @author Jafet Poco
  */
  BlockID getNext() const;
  /*

  /* * Obtiene el bloque actual.
   * @brief Devuelve el ID del bloque actual.
   * @return ID del bloque actual.
   * @author Berly Dueñas 
   */
  BlockID getCurrent() const;

  /* Avanza al siguiente bloque.
   * @brief Avanza al siguiente bloque del archivo.
   * @return true si se avanzó al siguiente bloque, false si no hay más bloques.
   * @author Jafet Poco
   */
  bool nextBlock();

  /* @brief Agrega un bloque si y solo si el nextblock es 0
   * @return true si se agregó el bloque, false en caso contrario.
   * @author Jafet Poco
   */
  bool addBlock();


  /* * Obtiene el nombre del archivo.
   * @brief Devuelve el nombre del archivo asociado al objeto File.
   * @return Nombre del archivo.
   * @author Jafet Poco
   */
  static void set_capacity(ssize_t c) { capacity=c; }

  /* * Obtiene la capacidad del archivo.
   * @brief Devuelve la capacidad del archivo.
   * @return Capacidad del archivo.
   * @author Jafet Poco
   */
  ssize_t getCapacity() const;
  //optional: bool remove();
};

#endif 
