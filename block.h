#ifndef BLOCK_H
#define BLOCK_H

#include "disk.h"
#include "globals.h"
#include <cstddef>
#include <string>
#include <vector>

using std::string;
using BlockID = ssize_t;

class Block {
private:
  BlockID id;
  size_t usedCapacity;
  string data;
public:
  /**
   * @brief Constructor por defecto
   * @author Jafet Poco
   */
  Block();
  /**
   * @brief Constructor que carga la información de un bloque existente
   * @param id Id del bloque
   * @author Berly Dueñas
   */
  Block(BlockID id);
  /**
   * @brief Carga la información de un bloque existente
   * @param id Id del bloque
   * @author Jafet Poco
   */
  void openBlock(BlockID id);
  /**
   * @brief Guarda la información del bloque en el disco, fragmentandolo en los sectores correspondientes
   * @author Berly Dueñas
   */
  void saveBlock();

  /**
   * @brief Devuelve la referencia a los datos del bloque
   * @return string& Referencia a los datos del bloque
   * @author Jafet Poco
   */
  string& getData() { return data; }

  // Getters
  BlockID getID() const { return id; }
  size_t getUsedCapacity() const { return usedCapacity; }

};

#endif 
